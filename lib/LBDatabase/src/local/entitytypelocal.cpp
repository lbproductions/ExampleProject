#include "entitytypelocal.h"

#include "attributelocal.h"
#include "column.h"
#include "contextlocal.h"
#include "database.h"
#include "entitylocal.h"
#include "functionlocal.h"
#include "../property.h"
#include "../propertyvalue.h"
#include "relationlocal.h"
#include "row.h"
#include "storagelocal.h"
#include "table.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** EntityTypePrivate
*/
//! \cond PRIVATE
const QString EntityTypeLocal::ContextColumn("context");
const QString EntityTypeLocal::IdentifierColumn("identifier");
const QString EntityTypeLocal::ParentEntityTypeColumn("parentEntityType");
const QString EntityTypeLocal::DisplayNameColumn("displayName");
const QString EntityTypeLocal::DisplayNamePluralColumn("displayNamePlural");
//! \endcond

class EntityTypeLocalPrivate {
    EntityTypeLocalPrivate() : context(0), parentEntityType(0), calculator(0) {}

    static QString typeToSql(AttributeLocal::Type type);

    void init();
    void inheritProperties(EntityTypeLocal *parent);
    void inheritCalculator(EntityTypeLocal *parent);
    AttributeLocal *addAttribute(const QString &identifier, AttributeLocal::Type type);
    RelationLocal *addRelation(const QString &identifier, EntityType *otherType, RelationLocal::Cardinality cardinality);

    Row *row;
    QString identifier;
    QString displayName;
    QString displayNamePlural;
    ContextLocal *context;
    StorageLocal *storage;
    EntityTypeLocal *parentEntityType;
    int parentEntityTypeId;

    QList<EntityType *> childEntityTypes;

    QHash<QString, Property *> propertiesByName;

    QList<Property *> properties;
    QList<Attribute *> attributes;
    QList<Relation *> relations;
    QList<Entity *> entities;
    QList<Function *> functions;

    Calculator *calculator;

    EntityTypeLocal * q_ptr;
    Q_DECLARE_PUBLIC(EntityTypeLocal)
};

QString EntityTypeLocalPrivate::typeToSql(AttributeLocal::Type type)
{
    switch(type) {
    case AttributeLocal::Text:
        return Column::typeToName(Column::Text);
    case AttributeLocal::Integer:
        return Column::typeToName(Column::Integer);
    case AttributeLocal::Real:
        return Column::typeToName(Column::Real);
    case AttributeLocal::Unkown:
    default:
        return Column::typeToName(Column::Blob);
    }
}

void EntityTypeLocalPrivate::init()
{
    Q_Q(EntityTypeLocal);
    identifier = row->data(EntityTypeLocal::IdentifierColumn).toString();
    displayName = row->data(EntityTypeLocal::DisplayNameColumn).toString();
    displayNamePlural = row->data(EntityTypeLocal::DisplayNamePluralColumn).toString();
    parentEntityTypeId = row->data(EntityTypeLocal::ParentEntityTypeColumn).toInt();
    int contextId = row->data(EntityTypeLocal::ContextColumn).toInt();
    context = static_cast<ContextLocal *>(storage->context(contextId));
    if(!context) {
        qWarning() << "No such context:" << contextId << "for entity type" << identifier;
        return;
    }
    context->addEntityType(q);
}

void EntityTypeLocalPrivate::inheritProperties(EntityTypeLocal *parent)
{
    Q_Q(EntityTypeLocal);

    QList<Relation *> newRelations = parent->relations();
    QList<Attribute *> newAttributes = parent->attributes();
    QList<Function *> newFunctions = parent->functions();

    properties.reserve(newAttributes.size() + newRelations.size() + newFunctions.size());
    attributes.reserve(newAttributes.size());
    relations.reserve(newRelations.size());
    functions.reserve(newFunctions.size());

    foreach(Attribute *attribute, newAttributes) {
        properties.append(attribute);
        propertiesByName.insert(attribute->identifier(), attribute);
    }
    foreach(Relation *r, newRelations) {
        RelationLocal *relation = static_cast<RelationLocal *>(r);
        properties.append(relation);
        propertiesByName.insert(relation->identifier(), relation);
    }
    foreach(Function *f, newFunctions) {
        FunctionLocal *function = static_cast<FunctionLocal *>(f);
        properties.append(function);
        propertiesByName.insert(function->identifier(), function);
    }

    relations.append(newRelations);
    attributes.append(newAttributes);
    functions.append(newFunctions);

    foreach(EntityType *type, childEntityTypes) {
        static_cast<EntityTypeLocal *>(type)->d_func()->inheritProperties(q);
    }
}

void EntityTypeLocalPrivate::inheritCalculator(EntityTypeLocal *parent)
{
    Q_Q(EntityTypeLocal);
    if(!calculator)
        calculator = parent->calculator();

    foreach(EntityType *type, childEntityTypes) {
        static_cast<EntityTypeLocal *>(type)->d_func()->inheritCalculator(q);
    }
}

AttributeLocal *EntityTypeLocalPrivate::addAttribute(const QString &name, AttributeLocal::Type type)
{
    Table *contextTable = storage->database()->table(context->identifier());
    contextTable->addColumn(name, EntityTypeLocalPrivate::typeToSql(type));

    Table *entitiesTable = storage->attributesTable();
    Row *row = entitiesTable->appendRow();
    row->setData(AttributeLocal::IdentifierColumn, QVariant(name));
    row->setData(AttributeLocal::DisplayNameColumn, QVariant(name));
    row->setData(AttributeLocal::EntityTypeIdColumn, QVariant(this->row->id()));

    AttributeLocal *attribute = new AttributeLocal(row, storage);
    storage->insertAttribute(attribute);
    attribute->addPropertyValueToEntities();
    foreach(Entity *entity, entities) {
        entity->propertyValue(attribute)->fetchValue();
    }

    return attribute;
}

RelationLocal *EntityTypeLocalPrivate::addRelation(const QString &name, EntityType *otherType, Relation::Cardinality cardinality)
{
//    Q_Q(EntityType);
//    Row *row = storage->contextsTable()->appendRow();
//    row->setData(Relation::NameColumn, QVariant(name));
//    row->setData(Relation::EntityTypeLeftColumn, QVariant(q->id()));
//    row->setData(Relation::EntityTypeRightColumn, QVariant(otherType->id()));
//    row->setData(Relation::DisplayNameLeftColumn, QVariant(name));
//    row->setData(Relation::DisplayNameRightColumn, QVariant(name));
//    row->setData(Relation::CardinalityColumn, QVariant(static_cast<int>(cardinality)));

//    if(cardinality == Relation::OneToMany || cardinality == Relation::OneToOne) {
//        storage->database()->table(otherType->identifier())->addColumn(name, Column::typeToName(Column::Integer));
//    }
//    else if(cardinality == Relation::ManyToMany) {
//        Table *table = storage->database()->createTable(name);
//        table->addColumn(q->identifier(), Column::typeToName(Column::Integer));
//        table->addColumn(otherType->identifier(), Column::typeToName(Column::Integer));
//    }

//    Relation *relation = new Relation(row, storage);
//    storage->insertRelation(relation);
//    relation->addPropertyValueToEntities();
//    foreach(Entity *entity, entities) {
//        entity->propertyValue(relation)->fetchValue();
//    }

//    return relation;
}

/******************************************************************************
** EntityType
*/
/*!
  \class EntityType
  \brief The EntityType class represents a description of a type of entities.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var EntityType::d_ptr
  \internal
  */

/*!
  \fn EntityType::nameChanged(QString name)

  This signal is emitted, when the name of the entity type changes.
  */

/*!
  Creates a new entity type, which represents the type described in \a row in
  the Storage \a parent.
  */
EntityTypeLocal::EntityTypeLocal(Row *row, StorageLocal *parent) :
    EntityType(parent),
    d_ptr(new EntityTypeLocalPrivate)
{
    Q_D(EntityTypeLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

/*!
  Destroys the type.
  */
EntityTypeLocal::~EntityTypeLocal()
{
}

/*!
  Returns the storage-global id of the type.
  */
int EntityTypeLocal::id() const
{
    Q_D(const EntityTypeLocal);
    return d->row->id();
}

/*!
  Returns the name of the type.
  */
QString EntityTypeLocal::identifier() const
{
    Q_D(const EntityTypeLocal);
    return d->identifier;
}

/*!
  Sets the name of the type to \a name.
  */
void EntityTypeLocal::setIdentifier(const QString &identifier)
{
    Q_D(EntityTypeLocal);
    if(d->identifier == identifier)
        return;

    d->row->setData(EntityTypeLocal::IdentifierColumn, QVariant(identifier));
    d->identifier = identifier;
    emit identifierChanged(identifier);
}

QString EntityTypeLocal::displayName() const
{
    Q_D(const EntityTypeLocal);
    return d->displayName;
}

void EntityTypeLocal::setDisplayName(const QString &displayName)
{
    Q_D(EntityTypeLocal);
    if(d->displayName == displayName)
        return;

    d->row->setData(EntityTypeLocal::DisplayNameColumn, QVariant(displayName));
    d->displayName = displayName;
    emit displayNameChanged(displayName);
}

QString EntityTypeLocal::displayNamePlural() const
{
    Q_D(const EntityTypeLocal);
    return d->displayNamePlural;
}

void EntityTypeLocal::setDisplayNamePlural(const QString &displayNamePlural)
{
    Q_D(EntityTypeLocal);
    if(d->displayNamePlural == displayNamePlural)
        return;

    d->row->setData(EntityTypeLocal::DisplayNamePluralColumn, QVariant(displayNamePlural));
    d->displayNamePlural = displayNamePlural;
    emit displayNameChanged(displayNamePlural);
}

/*!
  Returns the context to which the type belongs.
  */
LBDatabase::Context *EntityTypeLocal::context() const
{
    Q_D(const EntityTypeLocal);
    return d->context;
}

/*!
  Returns the type, from which this type inherits its properties or \a 0 if this
  type is the base type of its Context.
  */
EntityType *EntityTypeLocal::parentEntityType() const
{
    Q_D(const EntityTypeLocal);
    return d->parentEntityType;
}

/*!
  \internal

  Sets the Context to \a context. This is done when loading the storage.
  */
void EntityTypeLocal::setContext(LBDatabase::ContextLocal *context)
{
    Q_D(EntityTypeLocal);
    if(d->context == context)
        return;
    d->context = context;
}

/*!
  \internal

  Adds a type, which inherits this type. This is done when loading the storage.
  */
void EntityTypeLocal::addChildEntityType(EntityTypeLocal *type)
{
    Q_D(EntityTypeLocal);
    if(d->childEntityTypes.contains(type))
        return;

    d->childEntityTypes.append(type);
}

/*!
  \internal

  Sets the type from which this type inherits its properties. This is done when
  loading the storage.
  */
void EntityTypeLocal::setParentEntityType(EntityTypeLocal *type)
{
    Q_D(EntityTypeLocal);
    if(d->parentEntityType == type)
        return;
    d->parentEntityType = type;
}

/*!
  \internal

  Returns the ID of the parent type. This is used when loading the storage.
  */
int EntityTypeLocal::parentEntityTypeId() const
{
    Q_D(const EntityTypeLocal);
    return d->parentEntityTypeId;
}

/*!
  Returns the list of types, which are derived from the type.
  */
QList<EntityType *> EntityTypeLocal::childEntityTypes() const
{
    Q_D(const EntityTypeLocal);
    return d->childEntityTypes;
}

Property *EntityTypeLocal::property(const QString &name) const
{
    Q_D(const EntityTypeLocal);
    return d->propertiesByName.value(name, 0);
}

/*!
  Returns the list of properties of the type.

  This includes all attributes and relations.
  */
QList<Property *> EntityTypeLocal::properties() const
{
    Q_D(const EntityTypeLocal);
    return d->properties;
}

/*!
  Returns the list of attributes of the type.
  */
QList<Attribute *> EntityTypeLocal::attributes() const
{
    Q_D(const EntityTypeLocal);
    return d->attributes;
}

/*!
  Returns the list of relations of the type.
  */
QList<Relation *> EntityTypeLocal::relations() const
{
    Q_D(const EntityTypeLocal);
    return d->relations;
}

QList<Function *> EntityTypeLocal::functions() const
{
    Q_D(const EntityTypeLocal);
    return d->functions;
}

QList<Property *> EntityTypeLocal::nonInhertitedProperties() const
{
    QList<Property *> list = properties();
    if(parentEntityType()) {
        foreach(Property *property, parentEntityType()->properties()) {
            list.removeAll(property);
        }
    }
    return list;
}

QList<Attribute *> EntityTypeLocal::nonInhertitedAttributes() const
{
    QList<Attribute *> list = attributes();
    if(parentEntityType()) {
        foreach(Attribute *attribute, parentEntityType()->attributes()) {
            list.removeAll(attribute);
        }
    }
    return list;
}

QList<Relation *> EntityTypeLocal::nonInhertitedRelations() const
{
    QList<Relation *> list = relations();
    if(parentEntityType()) {
        foreach(Relation *relation, parentEntityType()->relations()) {
            list.removeAll(relation);
        }
    }
    return list;
}

QList<Function *> EntityTypeLocal::nonInhertitedFunctions() const
{
    QList<Function *> list = functions();
    if(parentEntityType()) {
        foreach(Function *function, parentEntityType()->functions()) {
            if(!function->reimplementingEntityTypes().contains(const_cast<EntityTypeLocal*>(this)))
                list.removeAll(function);
        }
    }
    return list;
}

/*!
  Adds a new attribute with the name \a name and the type \a type to the type.
  */
Attribute *EntityTypeLocal::addAttribute(const QString &name, AttributeLocal::Type type)
{
    Q_D(EntityTypeLocal);
    return d->addAttribute(name, type);
}

/*!
  Adds a new relation with the name \a name, which will map to entities of type
  \a otherType and has the cardinality \a cardinality.
  */
Relation *EntityTypeLocal::addRelation(const QString &name, EntityType *otherType, Relation::Cardinality cardinality)
{
    Q_D(EntityTypeLocal);
    return d->addRelation(name, otherType, cardinality);
}

/*!
  Returns a list of all entites of the type. This includes all entities of
  types, which inherit this type.
  */
QList<Entity *> EntityTypeLocal::entities() const
{
    Q_D(const EntityTypeLocal);
    return d->entities;
}

/*!
  Returns true, if the given \a entityType inherits or is equal to this type.
  This means, that the given type has at least the properties of this type.
  */
bool EntityTypeLocal::inherits(EntityType *entityType) const
{
    Q_D(const EntityTypeLocal);

    if(d->parentEntityType == entityType || this == entityType)
        return true;

    if(!d->parentEntityType || !entityType)
        return false;

    return d->parentEntityType->inherits(entityType);
}

Calculator *EntityTypeLocal::calculator() const
{
    Q_D(const EntityTypeLocal);
    return d->calculator;
}

/*!
  \internal

  Adds \a attribute to the attributes of this type. This is used when loading the storage.
  */
void EntityTypeLocal::addAttribute(AttributeLocal *attribute)
{
    Q_D(EntityTypeLocal);
    d->properties.append(attribute);
    d->propertiesByName.insert(attribute->identifier(), attribute);
    d->attributes.append(attribute);
}

/*!
  \internal

  Adds \a relation to the relations of this type. This is used when loading the storage.
  */
void EntityTypeLocal::addRelation(RelationLocal *relation)
{
    Q_D(EntityTypeLocal);
    d->properties.append(relation);
    d->propertiesByName.insert(relation->identifier(), relation);
    d->relations.append(relation);
}

void EntityTypeLocal::addFunction(FunctionLocal *function)
{
    Q_D(EntityTypeLocal);
    d->properties.append(function);
    d->propertiesByName.insert(function->identifier(), function);
    d->functions.append(function);
}

/*!
  \internal

  Adds the properties of \a parent to this entity.
  */
void EntityTypeLocal::inheritProperties(EntityTypeLocal *parent)
{
    Q_D(EntityTypeLocal);
    d->inheritProperties(parent);
}

void EntityTypeLocal::inheritCalculator(EntityTypeLocal *parent)
{
    Q_D(EntityTypeLocal);
    d->inheritCalculator(parent);
}

/*!
  \internal

  Adds \a entity to the list of all entities of this type and each type it is
  derived from.
  */
void EntityTypeLocal::addEntity(EntityLocal *entity)
{
    Q_D(EntityTypeLocal);
    d->entities.append(entity);
    if(d->parentEntityType)
        d->parentEntityType->addEntity(entity);
}

void EntityTypeLocal::setCalculator(Calculator *calculator)
{
    Q_D(EntityTypeLocal);
    d->calculator = calculator;
}

} // namespace LBDatabase
