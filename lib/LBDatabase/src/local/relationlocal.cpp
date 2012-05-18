#include "relation.h"

#include "attributelocal.h"
#include "column.h"
#include "contextlocal.h"
#include "database.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "relationvaluelocal.h"
#include "relationvaluelocal_p.h"
#include "row.h"
#include "storagelocal.h"
#include "table.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** RelationPrivate
*/
//! \cond PRIVATE
const QString RelationLocal::IdentifierColumn("identifier");
const QString RelationLocal::IdentifierRightColumn("identifierRight");
const QString RelationLocal::DisplayNameLeftColumn("displayName");
const QString RelationLocal::DisplayNameRightColumn("displayNameRight");
const QString RelationLocal::EntityTypeLeftColumn("entitytypeleft");
const QString RelationLocal::EntityTypeRightColumn("entitytyperight");
const QString RelationLocal::CardinalityColumn("cardinality");
const QString RelationLocal::TableNameColumn("tableName");
const QString RelationLocal::ColumnNameColumn("columnName");
const QString RelationLocal::ColumnNameRightColumn("columnNameRight");
const QString RelationLocal::EditableColumn("editable");
const QString RelationLocal::DirectionColumn("direction");

class RelationLocalPrivate {
protected:
    RelationLocalPrivate() :transposeRelation(0)
    {}

    virtual void init();
    void addPropertyValueToEntities();
    void initializeManyToManyRelation();
    void initializeOneToXRelation();
    void addPropertyValue(EntityLocal *entity);
    void fetchValues();
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    Row *row;
    StorageLocal *storage;

    QString identifier;
    QString tableName;
    QString columnName;
    QString displayName;
    EntityTypeLocal *entityType;
    EntityTypeLocal *entityTypeOther;
    RelationLocal::Cardinality cardinality;
    RelationLocal::Direction direction;
    bool editable;
    bool transpose;

    Table *relationTable;

    TransposeRelation *transposeRelation;
    QList<Property *> dependendProperties;

    RelationLocal * q_ptr;
    Q_DECLARE_PUBLIC(RelationLocal)
};

class TransposeRelationPrivate : public RelationLocalPrivate
{
    TransposeRelationPrivate() {}
    void init();

    Q_DECLARE_PUBLIC(TransposeRelation)
};

void RelationLocalPrivate::init()
{
    Q_Q(RelationLocal);

    identifier = row->data(RelationLocal::IdentifierColumn).toString();
    tableName = row->data(RelationLocal::TableNameColumn).toString();
    columnName = row->data(RelationLocal::ColumnNameColumn).toString();
    displayName = row->data(RelationLocal::DisplayNameLeftColumn).toString();
    entityType = storage->entityType(row->data(RelationLocal::EntityTypeLeftColumn).toInt());
    entityTypeOther = storage->entityType(row->data(RelationLocal::EntityTypeRightColumn).toInt());
    cardinality = static_cast<RelationLocal::Cardinality>(row->data(RelationLocal::CardinalityColumn).toInt());
    editable = row->data(RelationLocal::EditableColumn).toBool();
    direction = static_cast<RelationLocal::Direction>(row->data(RelationLocal::DirectionColumn).toInt());
    transpose = false;

    relationTable = storage->database()->table(tableName);

    if(!relationTable)
        qWarning() << "No such table" << tableName << "for relation" << identifier << row->id();

    if(direction == RelationLocal::Both)
        transposeRelation = new TransposeRelation(q);

    entityType->addRelation(q);
    static_cast<ContextLocal *>(entityType->context())->addRelation(q);
}

void RelationLocalPrivate::addPropertyValueToEntities()
{
    Q_Q(RelationLocal);
    foreach(Entity *entity, entityType->entities()) {
        static_cast<EntityLocal *>(entity)->addRelationValue(new RelationValueLocal<EntityLocal>(q, static_cast<EntityLocal *>(entity)));
    }

    if(transposeRelation)
        transposeRelation->addPropertyValueToEntities();
}

void RelationLocalPrivate::fetchValues()
{
    switch(cardinality) {
    case RelationLocal::OneToOne:
    case RelationLocal::OneToMany:
        initializeOneToXRelation();
        break;
    case RelationLocal::ManyToMany:
        initializeManyToManyRelation();
        break;
    }
}

void RelationLocalPrivate::addDependendProperty(Property *property)
{
    dependendProperties.append(property);
}

void RelationLocalPrivate::initializeManyToManyRelation()
{
    Q_Q(RelationLocal);
    Column *c1 = relationTable->column(columnName);
    if(!c1) {
        qWarning() << "No such column" << columnName << "for relation" << identifier << row->id();
    }

    Column *c2 = relationTable->column(static_cast<TransposeRelationPrivate *>(transposeRelation->d_func())->columnName);
    if(!c2) {
        qWarning() << "No such column" << static_cast<TransposeRelationPrivate *>(transposeRelation->d_func())->columnName << "for relation" << identifier << row->id();
    }

    int entityTypeLeftColumn = c1->index();
    int entityTypeRightColumn = c2->index();

    foreach(Row *row, relationTable->rows()) {
        int leftId = row->data(entityTypeLeftColumn).toInt();
        int rightId = row->data(entityTypeRightColumn).toInt();
        if(leftId > 0 && rightId > 0) {
            Entity *leftEntity = entityType->context()->entity(leftId);
            Entity *rightEntity = entityTypeOther->context()->entity(rightId);
            if(!leftEntity  || !rightEntity) {
                continue;
            }

            RelationValueBaseLocal *leftValue = static_cast<RelationValueBaseLocal *>(leftEntity->propertyValue(q));
            RelationValueBaseLocal *rightValue = static_cast<RelationValueBaseLocal *>(rightEntity->propertyValue(transposeRelation));
            leftValue->addOtherEntity(rightEntity);
            rightValue->addOtherEntity(leftEntity);
        }
    }
}

void RelationLocalPrivate::initializeOneToXRelation()
{
    Q_Q(RelationLocal);

    Column *column = relationTable->column(columnName);
    if(!column) {
        qWarning() << "No such column" << columnName << "for relation" << identifier << row->id();
    }

    int rightColumnIndex = relationTable->column(columnName)->index();

    int leftId;
    Entity *leftEntity;
    RelationValueBaseLocal *leftValue;
    RelationValueBaseLocal *rightValue;
    foreach(Entity *rightEntity, entityTypeOther->entities()) {
        leftId = static_cast<EntityLocal *>(rightEntity)->row()->data(rightColumnIndex).toInt();
        leftEntity = entityType->context()->entity(leftId);

        if(leftEntity) {
            leftValue = static_cast<RelationValueBaseLocal *>(leftEntity->propertyValue(q));
            leftValue->addOtherEntity(rightEntity);

            if(transposeRelation) {
                rightValue = static_cast<RelationValueBaseLocal *>(rightEntity->propertyValue(transposeRelation));
                rightValue->addOtherEntity(leftEntity);
            }
        }
    }
}

void RelationLocalPrivate::addPropertyValue(EntityLocal *entity)
{
    Q_Q(RelationLocal);
    entity->addRelationValue(new RelationValueLocal<EntityLocal>(q, entity));
}

void RelationLocalPrivate::connectDependendPropertyValues()
{
    Q_Q(RelationLocal);
    foreach(Property *property, dependendProperties) {
        foreach(Entity *dependendEntity, property->entityType()->entities()) {
            foreach(Entity *entity, q->entityType()->entities()) {
                QObject::connect(entity->propertyValue(q), SIGNAL(dataChanged(QVariant)),
                        dependendEntity->propertyValue(property), SLOT(calculate()));
            }
        }
    }
}

//! \endcond

/******************************************************************************
** Relation
*/
/*!
  \class Relation
  \brief The Relation class represents a description of a relation between two
  EntityTypes.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var Relation::d_ptr
  \internal
  */

/*!
  \enum Relation::Cardinality

  This enumeration describes the cardinality of a relation.

  */

/*!
  Creates a relation in the given \a storage, which is described by \a row.
  */
RelationLocal::RelationLocal(Row *row, StorageLocal *parent) :
    Relation(parent),
    d_ptr(new RelationLocalPrivate)
{
    Q_D(RelationLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

RelationLocal::RelationLocal(RelationLocalPrivate &dd, Row *row, StorageLocal *parent) :
    Relation(parent),
    d_ptr(&dd)
{
    Q_D(RelationLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

/*!
  Destroys the relation.
  */
RelationLocal::~RelationLocal()
{
}

/*!
  Returns the storage-global ID of the relation.
  */
int RelationLocal::id() const
{
    Q_D(const RelationLocal);
    return d->row->id();
}

/*!
  Returns the name of the relation. If the relation has a 1:N cardinality this
  is the name of the column in the left context, if the cardinatlity is N:M, it
  is the name of the table, which stores the pairs of the relation.
  */
QString RelationLocal::identifier() const
{
    Q_D(const RelationLocal);
    return d->identifier;
}

/*!
  Returns the display name in the given \a context, if its the left or right
  context or name(), if the context has nothing to do with the relation.
  */
QString RelationLocal::displayName() const
{
    Q_D(const RelationLocal);
    return d->displayName;
}

Property::Type RelationLocal::propertyType() const
{
    return Property::Relation;
}

/*!
  Sets the display name in the given \a context.

  Does nothing if the context has nothing to do with the context.
  */
void RelationLocal::setDisplayName(const QString &displayName)
{
    Q_D(RelationLocal);
    d->row->setData(DisplayNameLeftColumn, QVariant(displayName));
    emit displayNameChanged(displayName);
}

/*!
  Returns the entity type from which the relation maps.
  */
EntityType *RelationLocal::entityType() const
{
    Q_D(const RelationLocal);
    return d->entityType;
}

EntityType *RelationLocal::entityTypeOther() const
{
    Q_D(const RelationLocal);
    return d->entityTypeOther;
}

/*!
  Returns the cardinality of the relation.
  */
RelationLocal::Cardinality RelationLocal::cardinality() const
{
    Q_D(const RelationLocal);
    return d->cardinality;
}

RelationLocal::Direction RelationLocal::direction() const
{
    Q_D(const RelationLocal);
    return d->direction;
}

bool RelationLocal::isEditable() const
{
    Q_D(const RelationLocal);
    return d->editable;
}

bool RelationLocal::isTranspose() const
{
    Q_D(const RelationLocal);
    return d->transpose;
}

/*!
  \internal

  Adds a RelationValueLeft to each Entity of the left EntityType and a
  RelationValueRight to each Entity of the right one.
  */
void RelationLocal::addPropertyValueToEntities()
{
    Q_D(RelationLocal);
    return d->addPropertyValueToEntities();
}

/*!
  \internal

  Adds a RelationValueLeft or Right to \a entity, if its of the leftEntityType()
  respectively rightEntityType().

  Does nothing if \a entity is of unkown type.
  */
void RelationLocal::addPropertyValue(Entity *entity)
{
    Q_D(RelationLocal);
    return d->addPropertyValue(static_cast<EntityLocal *>(entity));
}

void RelationLocal::fetchValues()
{
    Q_D(RelationLocal);
    return d->fetchValues();
}

void RelationLocal::addDependendProperty(Property *property)
{
    Q_D(RelationLocal);
    d->addDependendProperty(property);
}

void RelationLocal::connectDependendPropertyValues()
{
    Q_D(RelationLocal);
    d->connectDependendPropertyValues();
}

Storage* RelationLocal::storage() const
{
    Q_D(const RelationLocal);
    return d->storage;
}

Row *RelationLocal::row() const
{
    Q_D(const RelationLocal);
    return d->row;
}

TransposeRelation::TransposeRelation(RelationLocal *relation) :
    RelationLocal(*new TransposeRelationPrivate, relation->row(), static_cast<StorageLocal *>(relation->storage()))
{
}

TransposeRelation::~TransposeRelation()
{
}

void TransposeRelationPrivate::init()
{
    Q_Q(RelationLocal);

    identifier = row->data(RelationLocal::IdentifierRightColumn).toString();
    tableName = row->data(RelationLocal::TableNameColumn).toString();
    columnName = row->data(RelationLocal::ColumnNameRightColumn).toString();
    displayName = row->data(RelationLocal::DisplayNameRightColumn).toString();
    entityType = storage->entityType(row->data(RelationLocal::EntityTypeRightColumn).toInt());
    entityTypeOther = storage->entityType(row->data(RelationLocal::EntityTypeLeftColumn).toInt());
    cardinality = static_cast<RelationLocal::Cardinality>(row->data(RelationLocal::CardinalityColumn).toInt());
    editable = row->data(RelationLocal::EditableColumn).toBool();
    direction = static_cast<RelationLocal::Direction>(row->data(RelationLocal::DirectionColumn).toInt());
    transpose = true;

    relationTable = storage->database()->table(tableName);

    entityType->addRelation(q);
    static_cast<ContextLocal *>(entityType->context())->addRelation(q);
}

} // namespace LBDatabase
