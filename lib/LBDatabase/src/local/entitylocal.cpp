#include "entitylocal.h"

#include "attributelocal.h"
#include "attributevaluelocal.h"
#include "contextlocal.h"
#include "entitytypelocal.h"
#include "functionvaluelocal.h"
#include "relationlocal.h"
#include "relationvaluelocal.h"
#include "row.h"
#include "storagelocal.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** EntityPrivate
*/
//! \cond PRIVATE
const QString EntityLocal::EntityTypeIdColumn("entityTypeId");
//! \endcond

class EntityLocalPrivate {
    EntityLocalPrivate() {}

    void init();
    void initializeRelations();
    void initializeRelationContent();

    StorageLocal *storage;
    ContextLocal *context;
    Row *row;
    EntityTypeLocal *entityType;
    QMultiHash<Property *, PropertyValue *> propertyValues;

    QList<AttributeValueLocal *> attributeValues;
    QList<RelationValueBaseLocal *> relationValues;
    QList<FunctionValueLocal *> functionValues;

    EntityLocal * q_ptr;
    Q_DECLARE_PUBLIC(EntityLocal)
};

void EntityLocalPrivate::init()
{
    Q_Q(EntityLocal);
    storage = static_cast<StorageLocal *>(context->storage());
    int id = row->data(EntityLocal::EntityTypeIdColumn).toInt();
    entityType = storage->entityType(id);
    entityType->addEntity(q);
}

/******************************************************************************
** Entity
*/
/*!
  \class Entity
  \brief The Entity class models a concrete instance of an EntityType.

  \ingroup highlevel-database-classes

  While EntityType represents a description for a group of entities, th Entity
  class is responsible for concrete single instances of such a type.

  Each entity has a set of PropertyValues, one value for each Property of the
  corresponding EntityType. You may access these values one by one via
  propertyValue or as a list propertyValues().

  You can create new entity instances with Context::insertEntity().
  */

/*!
  \var Entity::d_ptr
  \internal
  */

/*!
  Creates a new entity, which represents the data in \a row in the Context \a
  parent.
  */
EntityLocal::EntityLocal(Row *row, Context *parent) :
    Entity(static_cast<ContextLocal *>(parent)),
    d_ptr(new EntityLocalPrivate)
{
    Q_D(EntityLocal);
    d->q_ptr = this;
    d->context = static_cast<ContextLocal *>(parent);
    d->row = row;
    d->init();
}

/*!
  Destroys the entity.
  */
EntityLocal::~EntityLocal()
{
}

/*!
  Returns a display name for the entity.
  */
QString EntityLocal::displayName() const
{
    Q_D(const EntityLocal);
    return d->entityType->identifier()+QLatin1String(" ID: ")+QString::number(d->row->id());
}

int EntityLocal::id() const
{
    return row()->id();
}

/*!
  Returns the data for the Property \a property or an invalid QVariant, when the
  entity's EntityType has no such property.
  */
QVariant EntityLocal::data(Property *property) const
{
    Q_D(const EntityLocal);
    PropertyValue *propertyValue = d->propertyValues.value(property, 0);
    if(!propertyValue)
        return QVariant();

    return propertyValue->data();
}

/*!
  Sets the data for the Property \a propery to \a data and returns true upon
  success.

  Returns false, if no such property exists in the EntityType of this entity or
  if the property is not editable.
  */
bool EntityLocal::setData(const QVariant &data, Property *property)
{
    Q_D(const EntityLocal);

    PropertyValue *propertyValue = d->propertyValues.value(property, 0);
    if(!propertyValue)
        return false;

    return propertyValue->setData(data);
}

/*!
  Returns the EntityType of the entity.
  */
EntityType *EntityLocal::entityType() const
{
    Q_D(const EntityLocal);
    return d->entityType;
}

/*!
  Returns the Storage in which the Entity is stored.
  */
Storage *EntityLocal::storage() const
{
    Q_D(const EntityLocal);
    return d->storage;
}

/*!
  Returns the Context to which the entity belongs.
  */
Context *EntityLocal::context() const
{
    Q_D(const EntityLocal);
    return d->context;
}

/*!
  Returns a list of property values of the entity,
  */
QList<PropertyValue *> EntityLocal::propertyValues() const
{
    Q_D(const EntityLocal);
    return d->propertyValues.values();
}

/*!
  Returns the property value for the Property \a property or \a 0 if the
  EntityType has no such property.
  */
PropertyValue *EntityLocal::propertyValue(Property *property) const
{
    Q_D(const EntityLocal);
    return d->propertyValues.value(property, 0);
}

QVariant EntityLocal::value(const QString &name) const
{
    Q_D(const EntityLocal);
    PropertyValue *value = d->propertyValues.value(d->entityType->property(name), 0);
    if(!value)
        return QVariant();

    return value->data();
}

void EntityLocal::setValue(const QString &name, const QVariant &data)
{
    Q_D(const EntityLocal);
    PropertyValue *value = d->propertyValues.value(d->entityType->property(name), 0);
    if(!value || !value->isEditable())
        return;

    value->setData(data);
}

/*!
  Returns the Row, which stores the property values of this entity.
  */
Row *EntityLocal::row() const
{
    Q_D(const EntityLocal);
    return d->row;
}

RelationValue *EntityLocal::relationUncast(const QString &name) const
{
    return static_cast<LBDatabase::RelationValue *>(propertyValue(entityType()->property(name)));
}

FunctionValue *EntityLocal::function(const QString &name) const
{
    return static_cast<LBDatabase::FunctionValue *>(propertyValue(entityType()->property(name)));
}

/*!
  \internal
  Adds the AttributeValue \a value to this entity. This is done when loading the storage.
  */
void EntityLocal::addAttributeValue(AttributeValueLocal *value)
{
    Q_D(EntityLocal);
    d->propertyValues.insert(value->property(), value);
    d->attributeValues.append(value);
}

/*!
  \internal
  Adds the RelationValue \a value to this entity. This is done when loading the storage.
  */
void EntityLocal::addRelationValue(RelationValueBaseLocal *value)
{
    Q_D(EntityLocal);
    d->propertyValues.insert(value->property(), value);
    d->relationValues.append(value);
}

void EntityLocal::addFunctionValue(FunctionValueLocal *value)
{
    Q_D(EntityLocal);
    d->propertyValues.insert(value->property(), value);
    d->functionValues.append(value);
}

} // namespace LBDatabase
