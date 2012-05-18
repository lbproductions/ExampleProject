#include "attributevaluelocal.h"

#include "attributelocal.h"
#include "calculator.h"
#include "column.h"
#include "contextlocal.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "enumattributelocal.h"
#include "row.h"
#include "table.h"

#include <QVariant>

namespace LBDatabase {

/******************************************************************************
** AttributeValuePrivate
*/
class AttributeValueLocalPrivate {
    AttributeValueLocalPrivate() : cached(false) {}

    void init();
    void fetchValue();
    QVariant calculate();

    EntityLocal *entity;
    AttributeLocal *attribute;

    mutable QVariant cachedData;
    mutable bool cached;

    AttributeValueLocal * q_ptr;
    Q_DECLARE_PUBLIC(AttributeValueLocal)
};

void AttributeValueLocalPrivate::init()
{
    Q_Q(AttributeValueLocal);
    QObject::connect(q, SIGNAL(dataChanged(QVariant)), entity->context(), SLOT(onPropertyValueDataChanged(QVariant)));
}

void AttributeValueLocalPrivate::fetchValue()
{
}

QVariant AttributeValueLocalPrivate::calculate()
{
    Q_Q(AttributeValueLocal);
    Calculator *calculator = entity->entityType()->calculator();
    if(!calculator)
        return QVariant();

    return calculator->calculate(entity,q);
}

/******************************************************************************
** AttributeValue
*/
/*!
  \class AttributeValue
  \brief The AttributeValue class represents a value of an Attribute of a
  concrete Entity instance.

  \ingroup highlevel-database-classes

  \todo Document when done.
  */

/*!
  \var AttributeValue::d_ptr
  \internal
  */

/*!
  Creates an AttributeValue instance, which represents a conrete property with
  the Attribute description \a attribute of the Entity \a parent.
  */
AttributeValueLocal::AttributeValueLocal(AttributeLocal *attribute, EntityLocal *parent) :
    AttributeValue(parent),
    d_ptr(new AttributeValueLocalPrivate)
{
    Q_D(AttributeValueLocal);
    d->q_ptr = this;
    d->attribute = attribute;
    d->entity = parent;
    d->init();
}

/*!
  Destroys the attribute value.
  */
AttributeValueLocal::~AttributeValueLocal()
{
}

/*!
  Returns the Entity to which this attribute value belongs.
  */
Entity *AttributeValueLocal::entity() const
{
    Q_D(const AttributeValueLocal);
    return d->entity;
}

/*!
  Returns the value of the attribute.

  The \a role parameter is currently being ignored.
  */
QVariant AttributeValueLocal::data(int role) const
{
    Q_UNUSED(role);
    Q_D(const AttributeValueLocal);
    if(d->attribute->type() == AttributeLocal::Enum) {
        return static_cast<EnumAttributeLocal *>(d->attribute)->stringValue(d->entity->row()->data(d->attribute->columnIndex()).toInt());
    }
    else {
        if(!d->attribute->isCalculated())
            return d->entity->row()->data(d->attribute->columnIndex());

        if(d->attribute->cacheData()) {
            if(!d->cached) {
                d->cachedData = const_cast<AttributeValueLocalPrivate*>(d)->calculate();
                d->cached = true;
            }

            return d->cachedData;
        }

        return const_cast<AttributeValueLocalPrivate*>(d)->calculate();
    }

    return QVariant();
}

/*!
  Sets the value of the attribute to \a data.

  Does nothing if the attribute is not editable.
  */
bool AttributeValueLocal::setData(const QVariant &data)
{
    Q_D(AttributeValueLocal);
    if(!isEditable())
        return false;

    d->entity->row()->setData(d->attribute->identifier(), data);
    emit dataChanged(data);
    return true;
}

/*!
  Returns true.
  */
bool AttributeValueLocal::isEditable() const
{
    Q_D(const AttributeValueLocal);
    return !d->attribute->isCalculated() && d->attribute->isEditable();
}

/*!
  Returns the Property instance, which describes this attribute value.
  */
Property *AttributeValueLocal::property() const
{
    Q_D(const AttributeValueLocal);
    return d->attribute;
}

/*!
  Reads the value of the attribute from the database.
  */
void AttributeValueLocal::fetchValue()
{
    Q_D(AttributeValueLocal);
    d->fetchValue();
}

void AttributeValueLocal::calculate()
{
    Q_D(AttributeValueLocal);
    d->calculate();
}

} // namespace LBDatabase
