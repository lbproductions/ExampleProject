#include "contextlocal.h"

#include "attributelocal.h"
#include "attributevaluelocal.h"
#include "calculator.h"
#include "database.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "functionlocal.h"
#include "relationlocal.h"
#include "row.h"
#include "storagelocal.h"
#include "table.h"

#include <QHash>
#include <QList>
#include <QDebug>
#include <QIcon>
#include <QPixmap>

namespace LBDatabase {

/******************************************************************************
** ContextPrivate
*/
//! \cond PRIVATE
const QString ContextLocal::IdentifierColumn("identifier");
const QString ContextLocal::DisplayNameColumn("displayName");
const QString ContextLocal::TableNameColumn("tableName");
//! \endcond

class ContextLocalPrivate {
    ContextLocalPrivate() : baseEntityType(0) {}

    void init();
    void initializeEntityHierarchy();
    void loadEntities();
    void initializeRelations();
    void fillRelations();
    void createBaseEntityType(const QString &identifier);

    EntityTypeLocal *addEntityType(const QString &identifier, EntityTypeLocal *parentEntityType);
    EntityLocal *insertEntity(EntityTypeLocal *type);

    EntityLocal *createEntityInstance(Row *row);
    Calculator *createCalculatorInstance(const QString &entityTypeName);

    Row *row;
    StorageLocal *storage;
    QString identifier;
    QString displayName;
    QList<EntityType *> entityTypes;
    EntityTypeLocal *baseEntityType;
    Table *contextTable;
    QList<Entity *> entities;
    QHash<int, EntityLocal *> entitiesById;
    QList<Property *> properties;

    QHash<QString, QMetaObject> entityMetaObjects;
    QHash<QString, QMetaObject> calculatorMetaObjects;

    ContextLocal * q_ptr;
    Q_DECLARE_PUBLIC(ContextLocal)
};

void ContextLocalPrivate::init()
{
    identifier = row->data(ContextLocal::IdentifierColumn).toString();
    displayName = row->data(ContextLocal::DisplayNameColumn).toString();
    contextTable = storage->database()->table(row->data(ContextLocal::TableNameColumn).toString());

    if(!contextTable)
        qWarning() << "No such table" << row->data(ContextLocal::TableNameColumn).toString() << "for context" << identifier << "(ID" << row->id() << ")";
}

void ContextLocalPrivate::initializeEntityHierarchy()
{
    EntityTypeLocal *parentType;
    foreach(EntityType *t, entityTypes) {
        EntityTypeLocal *type = static_cast<EntityTypeLocal *>(t);

        parentType = storage->entityType(type->parentEntityTypeId());
        if(parentType) {
            type->setParentEntityType(parentType);
            parentType->addChildEntityType(type);
        }
        else {
            baseEntityType = type;
        }
    }
    foreach(EntityType *t, entityTypes) {
        EntityTypeLocal *type = static_cast<EntityTypeLocal *>(t);
        type->setCalculator(createCalculatorInstance(type->identifier()));
    }

    foreach(EntityType *c, baseEntityType->childEntityTypes()) {
        EntityTypeLocal *child = static_cast<EntityTypeLocal *>(c);
        child->inheritProperties(baseEntityType);
        child->inheritCalculator(baseEntityType);
    }
}

void ContextLocalPrivate::loadEntities()
{
    if(!contextTable)
        return;

    entities.reserve(contextTable->rows().size());
    entitiesById.reserve(contextTable->rows().size());
    foreach(Row *row, contextTable->rows()) {
        EntityLocal *entity = createEntityInstance(row);
        if(entity) {
            entities.append(entity);
            entitiesById.insert(row->id(), entity);
        }
    }
}

EntityTypeLocal *ContextLocalPrivate::addEntityType(const QString &name, EntityTypeLocal *parentEntityType)
{
    Row *entityTypeRow = storage->entityTypesTable()->appendRow();
    entityTypeRow->setData(EntityTypeLocal::IdentifierColumn, QVariant(name));
    entityTypeRow->setData(EntityTypeLocal::ParentEntityTypeColumn, QVariant(parentEntityType->id()));
    entityTypeRow->setData(EntityTypeLocal::ContextColumn, QVariant(row->id()));

    EntityTypeLocal *type = new EntityTypeLocal(entityTypeRow, storage);
    type->setParentEntityType(parentEntityType);
    parentEntityType->addChildEntityType(type);
    storage->insertEntityType(type);

    type->inheritProperties(parentEntityType);
    return type;
}

EntityLocal *ContextLocalPrivate::insertEntity(EntityTypeLocal *type)
{
    Q_Q(ContextLocal);
    Row *row = contextTable->appendRow();
    row->setData(EntityLocal::EntityTypeIdColumn, QVariant(type->id()));

    q->beginInsertRows(QModelIndex(), entities.size(), entities.size());
    EntityLocal *entity =  createEntityInstance(row);
    entities.append(entity);
    entitiesById.insert(row->id(), entity);

    foreach(Property *property, type->properties()) {
        property->addPropertyValue(entity);
    }
    foreach(PropertyValue *value, entity->propertyValues()) {
        value->fetchValue();
    }
    q->endInsertRows();

    return entity;
}

void ContextLocalPrivate::createBaseEntityType(const QString &name)
{
    Row *entityTypeRow = storage->entityTypesTable()->appendRow();
    entityTypeRow->setData(EntityTypeLocal::IdentifierColumn, QVariant(name));
    entityTypeRow->setData(EntityTypeLocal::ParentEntityTypeColumn, QVariant());
    entityTypeRow->setData(EntityTypeLocal::ContextColumn, QVariant(row->id()));

    baseEntityType = new EntityTypeLocal(entityTypeRow, storage);
    storage->insertEntityType(baseEntityType);
}

EntityLocal *ContextLocalPrivate::createEntityInstance(Row *row)
{
    Q_Q(ContextLocal);
    int typeId = row->data(EntityLocal::EntityTypeIdColumn).toInt();
    EntityType *type = storage->entityType(typeId);
    if(!type)
        return 0;

    QString entityTypeName = type->identifier();

    while(!entityMetaObjects.contains(entityTypeName)) {
        type = type->parentEntityType();
        if(!type)
            break;

        entityTypeName = type->identifier();
    }

    if(!entityMetaObjects.contains(entityTypeName))
        return new EntityLocal(row, q);

    QObject *object = entityMetaObjects.value(entityTypeName).newInstance(Q_ARG(::LBDatabase::Row*,row), Q_ARG(::LBDatabase::ContextLocal*, q));
    return static_cast<EntityLocal *>(object);
}

Calculator *ContextLocalPrivate::createCalculatorInstance(const QString &entityTypeName)
{
    Q_Q(ContextLocal);
    if(!calculatorMetaObjects.contains(entityTypeName))
        return 0;

    QObject *object = calculatorMetaObjects.value(entityTypeName).newInstance(Q_ARG(QObject*, q));
    return static_cast<Calculator *>(object);
}


/******************************************************************************
** Context
*/
/*!
  \class Context
  \brief The Context class represents a context in a Storage.

  \ingroup highlevel-database-classes

  Each context is responsible for a hierarchy of EntityType and the concrete
  Entity instances of these types. It always has exactly one baseEntityType()
  from which other types in the context may be derived. The entityTypes() list
  contains every type in the context. You can add a new type with
  addEntityType().

  You may access the Entity instances of each context either by id (entity()),
  by index (entityAt()) or through the list of all entities(). You can insert
  new entities into the context by calling insertEntity().

  The Context class inherits QAbstractTableModel, so that you can add it as a
  model for any model-view based Qt class like QTreeView. This model is
  editable.

  */
/*!
  \fn Context::nameChanged(QString name)

  This signal is emitted when the name of this context changes.

  */

/*!
  \var Context::d_ptr
  \internal
  */

/*!
  Creates a new context in the Storage \a parent, which is described by /a row.
  */
ContextLocal::ContextLocal(Row *row, StorageLocal *parent) :
    Context(parent),
    d_ptr(new ContextLocalPrivate)
{
    Q_D(ContextLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

/*!
  Destroys the context.
  */
ContextLocal::~ContextLocal()
{
}

/*!
  Returns the storage-global ID of the context.
  */
int ContextLocal::id() const
{
    Q_D(const ContextLocal);
    return d->row->id();
}

/*!
  Returns the name of the context. This name is also the name of the Sqlite
  table, which contains the Entity instances of the context.
  */
QString ContextLocal::identifier() const
{
    Q_D(const ContextLocal);
    return d->identifier;
}

QString ContextLocal::displayName() const
{
    Q_D(const ContextLocal);
    return d->displayName;
}

/*!
  Returns the storage, which contains the context.
  */
Storage *ContextLocal::storage() const
{
    Q_D(const ContextLocal);
    return d->storage;
}

/*!
  Returns the table, that contains the entities in this context.
  */
Table *ContextLocal::table() const
{
    Q_D(const ContextLocal);
    return d->contextTable;
}

/*!
  Returns the EntityType, from which all types in the context are somehow
  derived.
  */
EntityType *ContextLocal::baseEntityType() const
{
    Q_D(const ContextLocal);
    return d->baseEntityType;
}

/*!
  Returns a list of all types, for which the context is responsible.
  */
QList<EntityType *> ContextLocal::entityTypes() const
{
    Q_D(const ContextLocal);
    return d->entityTypes;
}

/*!
  Adds a new EntityType to the context, which will inherit all properties from
  \a parentEntityType.
  */
EntityType *ContextLocal::addEntityType(const QString &name, EntityType *parentEntityType)
{
    Q_D(ContextLocal);
    return d->addEntityType(name, static_cast<EntityTypeLocal *>(parentEntityType));
}

/*!
  Returns the Entity instance with the ID \a id.
  */
Entity *ContextLocal::entity(int id) const
{
    Q_D(const ContextLocal);
    return d->entitiesById.value(id);
}

/*!
  Returns a list of all Entity instances.
  */
QList<Entity *> ContextLocal::entities() const
{
    Q_D(const ContextLocal);
    return d->entities;
}

/*!
  Inserts a new Entity of the type \a type into the context and returns the
  instance.
  */
Entity *ContextLocal::insertEntity(EntityType *type)
{
    Q_D(ContextLocal);
    return d->insertEntity(static_cast<EntityTypeLocal *>(type));
}

/*!
  \internal

  Creates the base entity type for the context. This is only being used, when
  creating a new context.
  */
void ContextLocal::createBaseEntityType(const QString &name)
{
    Q_D(ContextLocal);
    d->createBaseEntityType(name);
}

/*!
  \internal

  Adds the EntityType \a type to the context. This is when loading the storage.
  */
void ContextLocal::addEntityType(EntityTypeLocal *type)
{
    Q_D(ContextLocal);
    if(d->entityTypes.contains(type))
        return;

    connect(type, SIGNAL(displayNameChanged(QString)), this, SLOT(onEntityTypeNameChanged(QString)));
    d->entityTypes.append(type);
}

/*!
  \internal

  Adds the Attribute \a attribute to the context. This is when loading the storage.
  */
void ContextLocal::addAttribute(AttributeLocal *attribute)
{
    Q_D(ContextLocal);
    if(d->properties.contains(attribute))
        return;

    beginInsertColumns(QModelIndex(), d->properties.size(), d->properties.size());
    d->properties.append(attribute);
    connect(attribute, SIGNAL(displayNameChanged(QString)), this, SLOT(onPropertyDisplayNameChanged(QString)));
    endInsertColumns();
}

void ContextLocal::addFunction(FunctionLocal *function)
{
    Q_D(ContextLocal);
    if(d->properties.contains(function))
        return;

    beginInsertColumns(QModelIndex(), d->properties.size(), d->properties.size());
    d->properties.append(function);
    connect(function, SIGNAL(displayNameChanged(QString)), this, SLOT(onPropertyDisplayNameChanged(QString)));
    endInsertColumns();
}

/*!
  \internal

  Adds the Relation \a relation to the context. This is when loading the storage.
  */
void ContextLocal::addRelation(RelationLocal *relation)
{
    Q_D(ContextLocal);
    if(d->properties.contains(relation))
        return;

    beginInsertColumns(QModelIndex(), d->properties.size(), d->properties.size());
    d->properties.append(relation);
    connect(relation, SIGNAL(displayNameChanged(QString)), this, SLOT(onPropertyDisplayNameChanged(QString)));
    endInsertColumns();
}

/*!
  \internal

  Initialized the hierarchy of the EntityTypes. This is when loading the storage.
  */
void ContextLocal::initializeEntityHierarchy()
{
    Q_D(ContextLocal);
    d->initializeEntityHierarchy();
}

/*!
  \internal

  Loads the Entity instances in this context. This is when loading the storage.
  */
void ContextLocal::loadEntities()
{
    Q_D(ContextLocal);
    d->loadEntities();
}

/*!
  Implements QAbstractTableModel::data()
  */
QVariant ContextLocal::data(const QModelIndex &index, int role) const
{
    Q_D(const ContextLocal);
    if(role == Qt::DecorationRole && index.column() > 1) {
        //        Entity *entity = d->entities.at(index.row());
        //        Property *property = d->properties.at(index.column() - 2);
        //        if(property->propertyType() == Property::Attribute) {
        //            Attribute *attribute = static_cast<Attribute *>(property);
        //            if(attribute->type() == Attribute::Icon) {
        //                QImage image(entity->data(attribute).toString());
        //                qDebug() << image.isNull();
        //                QPixmap pixmap(entity->data(attribute).toString());
        //                qDebug() << pixmap.isNull();
        //                return image;
        //            }
        //            else if(attribute->type() == Attribute::Pixmap) {
        //                return QImage(entity->data(attribute).toString());
        //            }
        //        }
    }
    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        Entity *entity = d->entities.at(index.row());
        switch(index.column()) {
        case 0:
            return entity->id();
        case 1:
            return entity->entityType()->displayName();
        default:
            Property *property = d->properties.at(index.column() - 2);
            PropertyValue *value = entity->propertyValue(property);
            if(!value) {
                return QVariant();
            }
            return value->data(role);
        }
    }


    return QVariant();
}

/*!
  Implements QAbstractTableModel::headerData()
  */
QVariant ContextLocal::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        if(role == Qt::DisplayRole) {
            Q_D(const ContextLocal);
            switch(section) {
            case 0:
                return QLatin1String("ID");
            case 1:
                return QLatin1String("Type");
            default:
                return d->properties.at(section - 2)->displayName();
            }
        }
        else if(role == Qt::TextAlignmentRole) {
            return Qt::AlignLeft;
        }
    }
    return QVariant();
}

/*!
  Implements QAbstractTableModel::columnCount()
  */
int ContextLocal::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    Q_D(const ContextLocal);
    return d->properties.size() + 2;
}

/*!
  Implements QAbstractTableModel::rowCount()
  */
int ContextLocal::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()) {
        return 0;
    }
    Q_D(const ContextLocal);
    return d->entities.size();
}

/*!
  \internal
  Listens to name changes of EntityTypes and updates the model accordingly.
  */
void ContextLocal::onEntityTypeNameChanged(QString name)
{
    Q_D(const ContextLocal);
    Q_UNUSED(name);
    EntityTypeLocal *type = static_cast<EntityTypeLocal *>(sender());
    QModelIndex i = index(d->entityTypes.indexOf(type), 1);
    emit dataChanged(i, i);
}

/*!
  \internal
  Listens to name changes of Properties and updates the model accordingly.
  */
void ContextLocal::onPropertyDisplayNameChanged(QString displayName)
{
    Q_D(const ContextLocal);
    Q_UNUSED(displayName);

    Property *p = static_cast<Property *>(sender());
    int i =d->properties.indexOf(p);
    emit headerDataChanged(Qt::Horizontal, i, i);
}

/*!
  \internal
  Listens to changes of PropertyValues and updates the model accordingly.
  */
void ContextLocal::onPropertyValueDataChanged(QVariant data)
{
    Q_D(const ContextLocal);
    Q_UNUSED(data);

    PropertyValue *v = static_cast<PropertyValue *>(sender());
    QModelIndex i = index(d->entities.indexOf(v->entity()), d->properties.indexOf(v->property()));
    emit dataChanged(i, i);
}

/*!
  Implements QAbstractTableModel::setData()
  */
bool ContextLocal::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(role == Qt::EditRole) {
        Q_D(const ContextLocal);
        Entity *e = d->entities.at(index.row());
        PropertyValue *v = e->propertyValue(d->properties.value(index.column() - 2));
        if(v)
            return v->setData(value);
    }
    return false;
}

/*!
  Implements QAbstractTableModel::flags()
  */
Qt::ItemFlags ContextLocal::flags(const QModelIndex &index) const
{
    Q_D(const ContextLocal);
    if(index.column() > 1) {
        Entity *e = d->entities.at(index.row());
        PropertyValue *v = e->propertyValue(d->properties.value(index.column() - 2));
        if(v && v->isEditable()) {
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        }
    }

    return QAbstractItemModel::flags(index);
}

void ContextLocal::registerEntityClass(const QString &entityName, QMetaObject metaObject)
{
    Q_D(ContextLocal);
    if(d->entityMetaObjects.contains(entityName))
        return;

    d->entityMetaObjects.insert(entityName, metaObject);
}

void ContextLocal::registerCalculatorClass(const QString &entityName, QMetaObject metaObject)
{
    Q_D(ContextLocal);
    if(d->calculatorMetaObjects.contains(entityName))
        return;

    d->calculatorMetaObjects.insert(entityName, metaObject);
}

} // namespace LBDatabase
