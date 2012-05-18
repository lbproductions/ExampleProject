#include "attributelocal.h"
#include "attributelocal_p.h"

#include "attributevaluelocal.h"
#include "column.h"
#include "contextlocal.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "row.h"
#include "storagelocal.h"
#include "table.h"

#include <QStringList>
#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** AttributePrivate
*/
void AttributeLocalPrivate::init()
{
    Q_Q(AttributeLocal);
    identifier = row->data(AttributeLocal::IdentifierColumn).toString();
    displayName = row->data(AttributeLocal::DisplayNameColumn).toString();
    calculated = row->data(AttributeLocal::CalculatedColumn).toBool();
    cacheData = row->data(AttributeLocal::CacheDataColumn).toBool();
    editable = row->data(AttributeLocal::EditableColumn).toBool();

    type = static_cast<AttributeLocal::Type>(row->data(AttributeLocal::TypeColumn).toInt());

    entityType = storage->entityType(row->data(AttributeLocal::EntityTypeIdColumn).toInt());
    if(!entityType) {
        qWarning() << "No such entity type:" << row->data(AttributeLocal::EntityTypeIdColumn).toInt();
        return;
    }

    if(!calculated && !static_cast<ContextLocal *>(entityType->context())->table()->column(identifier)) {
        qWarning() << "No such column:" << identifier << "in table" << static_cast<ContextLocal *>(entityType->context())->table()->name();
        return;
    }

    columnIndex = -1;
    if(!calculated)
        columnIndex = static_cast<ContextLocal *>(entityType->context())->table()->column(identifier)->index();
    entityType->addAttribute(q);
    static_cast<ContextLocal *>(entityType->context())->addAttribute(q);
}

void AttributeLocalPrivate::addPropertyValueToEntities()
{
    foreach(Entity *entity, entityType->entities()) {
        addPropertyValue(entity);
    }
}

void AttributeLocalPrivate::addPropertyValue(Entity *entity)
{
    Q_Q(AttributeLocal);
    static_cast<EntityLocal *>(entity)->addAttributeValue(new AttributeValueLocal(q, static_cast<EntityLocal *>(entity)));
}

void AttributeLocalPrivate::fetchValues()
{
}

void AttributeLocalPrivate::addDependendProperty(Property *property)
{
    dependendProperties.append(property);
}

void AttributeLocalPrivate::connectDependendPropertyValues()
{
    Q_Q(AttributeLocal);
    foreach(Property *property, dependendProperties) {
        foreach(Entity *dependendEntity, property->entityType()->entities()) {
            foreach(Entity *entity, q->entityType()->entities()) {
                QObject::connect(entity->propertyValue(q), SIGNAL(dataChanged(QVariant)),
                        dependendEntity->propertyValue(property), SLOT(calculate()));
            }
        }
    }
}

/******************************************************************************
** Attribute
*/
/*!
  \class Attribute
  \brief The Attribute class represents a simple single-value property of an
  EntityType.

  \ingroup highlevel-database-classes

  \todo Document when class is done.

  */

/*!
  \var Attribute::d_ptr
  \internal
  */

/*!
  \enum Attribute::Type

  This enumeration describes the type stored in the attribute.

  */

/*!
  \enum Attribute::PrefetchStrategy
  \brief Describes if and when the value of an attribute will be prefetched.
  */

/*!
  The name of 'name' column.
  */
const QString AttributeLocal::IdentifierColumn("identifier");
/*!
  The name of 'displayName' column.
  */
const QString AttributeLocal::DisplayNameColumn("displayName");
/*!
  The name of 'entityTypeId' column.
  */
const QString AttributeLocal::EntityTypeIdColumn("entityTypeId");
const QString AttributeLocal::CalculatedColumn("calculated");
const QString AttributeLocal::CacheDataColumn("cacheData");
const QString AttributeLocal::TypeColumn("type");
const QString AttributeLocal::EditableColumn("editable");

/*!
  Creates an attribute, which contains the meta data from \a row in the given \a
  storage.
  */
AttributeLocal::AttributeLocal(Row *row, StorageLocal *parent) :
    Attribute(parent),
    d_ptr(new AttributeLocalPrivate)
{
    Q_D(AttributeLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

AttributeLocal::AttributeLocal(AttributeLocalPrivate &dd, Row *row, StorageLocal *parent) :
    Attribute(parent),
    d_ptr(&dd)
{
    Q_D(AttributeLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

/*!
  Adds an AttributeValue instance to every existing Entity of the corrent
  EntityType.
  */
void AttributeLocal::addPropertyValueToEntities()
{
    Q_D(AttributeLocal);
    d->addPropertyValueToEntities();
}

/*!
  Adds an AttributeValue instance to the given \a entity.
  */
void AttributeLocal::addPropertyValue(Entity *entity)
{
    Q_D(AttributeLocal);
    d->addPropertyValue(static_cast<EntityLocal *>(entity));
}

void AttributeLocal::addDependendProperty(Property *property)
{
    Q_D(AttributeLocal);
    d->addDependendProperty(property);
}

void AttributeLocal::connectDependendPropertyValues()
{
    Q_D(AttributeLocal);
    d->connectDependendPropertyValues();
}

void AttributeLocal::fetchValues()
{
    Q_D(AttributeLocal);
    d->fetchValues();
}

/*!
  Destroys the attribute.
  */
AttributeLocal::~AttributeLocal()
{
}

/*!
  Returns the storage-global ID of the attribute.
  */
int AttributeLocal::id() const
{
    Q_D(const AttributeLocal);
    return d->row->id();
}

EntityType *AttributeLocal::entityType() const
{
    Q_D(const AttributeLocal);
    return d->entityType;
}

/*!
  Returns the name of the attribute. if the attribute is stored in the database
  this name is the name of the column where it is stored.
  */
QString AttributeLocal::identifier() const
{
    Q_D(const AttributeLocal);
    return d->identifier;
}

bool AttributeLocal::isCalculated() const
{
    Q_D(const AttributeLocal);
    return d->calculated;
}

bool AttributeLocal::cacheData() const
{
    Q_D(const AttributeLocal);
    return d->cacheData;
}

bool AttributeLocal::isEditable() const
{
    Q_D(const AttributeLocal);
    return d->editable;
}

Property::Type AttributeLocal::propertyType() const
{
    return Property::Attribute;
}

/*!
  Returns the display name of the attribute. Since each Attribute can only be
  part of one Context, the \a context parameter will be ignored.
  */
QString AttributeLocal::displayName() const
{
    Q_D(const AttributeLocal);
    return d->displayName;
}

/*!
  Sets the display name of the attribute to \a displayName. Since each Attribute
  can only be part of one Context, the \a context parameter will be ignored.
  */
void AttributeLocal::setDisplayName(const QString &displayName)
{
    Q_D(AttributeLocal);
    if(d->displayName == displayName)
        return;

    d->row->setData(AttributeLocal::DisplayNameColumn, QVariant(displayName));
    d->displayName = displayName;
    emit displayNameChanged(displayName);
}

/*!
  Return the internal index of the column of this attribute.
  */
int AttributeLocal::columnIndex() const
{
    Q_D(const AttributeLocal);
    return d->columnIndex;
}

AttributeLocal::Type AttributeLocal::type() const
{
    Q_D(const AttributeLocal);
    return d->type;
}

QString AttributeLocal::typeName() const
{
    Q_D(const AttributeLocal);
    return AttributeLocal::typeToName(d->type);
}

QString AttributeLocal::typeToName(Type type)
{
    return AttributeLocal::typeNames().at(static_cast<int>(type));
}

QStringList AttributeLocal::typeNames()
{
    QStringList names;
    names << "Unkown" <<
    "Text" <<
    "Integer" <<
    "Real" <<
    "Icon" <<
    "Pixmap" <<
    "DateTime" <<
    "Time" <<
    "Bool" <<
    "Color" <<
    "Enum";
    return names;
}

QString AttributeLocal::qtType() const
{
    Q_D(const AttributeLocal);
    return AttributeLocal::typeToQtType(d->type);
}

QString AttributeLocal::typeToQtType(Type type)
{
    return AttributeLocal::qtTypeNames().at(static_cast<int>(type));
}

QStringList AttributeLocal::qtTypeNames()
{
    QStringList names;
    names << "QVariant" << //Unknown
    "QString" << //Text
    "int" << //Integer
    "double" << //Real
    "QIcon" << //Icon
    "QPixmap" << //Pixmap
    "QDateTime" << //DateTime
    "QTime" << //Time
    "bool" << //Bool
    "QColor" << //Color
    "Enum"; //Enum
    return names;
}

} // namespace LBDatabase
