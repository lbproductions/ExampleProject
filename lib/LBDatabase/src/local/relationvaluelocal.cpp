#include "relationvaluelocal.h"
#include "relationvaluelocal_p.h"

#include "contextlocal.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "relationlocal.h"
#include "row.h"

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** RelationValuePrivate
*/

//! \cond PRIVATE
void RelationValueBaseLocalPrivate::init()
{
    Q_Q(RelationValueBaseLocal);
    QObject::connect(q, SIGNAL(dataChanged(QVariant)), static_cast<ContextLocal *>(entity->context()), SLOT(onPropertyValueDataChanged(QVariant)));
}

void RelationValueBaseLocalPrivate::fetchValue()
{
}

//! \endcond

/******************************************************************************
** RelationValue
*/
/*!
  \class RelationValue
  \brief The RelationValue class represents a value of a relation between two
  EntityTypes.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var RelationValue::d_ptr
  \internal
  */

/*!
  Creates a RelationValue.
  */
RelationValueBaseLocal::RelationValueBaseLocal(RelationLocal *relation, EntityLocal *parent) :
    RelationValue(parent),
    d_ptr(new RelationValueBaseLocalPrivate)
{
    Q_D(RelationValueBaseLocal);
    d->q_ptr = this;
    d->relation = relation;
    d->entity = parent;
    d->init();
}

/*!
  Destroys the relation value.
  */
RelationValueBaseLocal::~RelationValueBaseLocal()
{
}

/*!
  Returns the entity of this relation value.
  */
Entity *RelationValueBaseLocal::entity() const
{
    Q_D(const RelationValueBaseLocal);
    return d->entity;
}

/*!
  Does nothing.
  */
bool RelationValueBaseLocal::setData(const QVariant &data)
{
    Q_UNUSED(data);
    return false;
}

/*!
  Returns false
  */
bool RelationValueBaseLocal::isEditable() const
{
    Q_D(const RelationValueBaseLocal);
    return d->relation->isEditable();
}

/*!
  Returns the relation, that describes this value.
  */
Property *RelationValueBaseLocal::property() const
{
    Q_D(const RelationValueBaseLocal);
    return d->relation;
}

/*!
  Initializes the content of the relation value.
  */
void RelationValueBaseLocal::fetchValue()
{
    Q_D(RelationValueBaseLocal);
    d->fetchValue();
}

void RelationValueBaseLocal::calculate()
{
}

} // namespace LBDatabase
