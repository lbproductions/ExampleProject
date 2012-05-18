#ifndef LBDATABASE_CONCRETERELATION_H
#define LBDATABASE_CONCRETERELATION_H

#include "relationlocal.h"

#include "entitylocal.h"
#include "entitytypelocal.h"
#include "relationvaluelocal.h"

namespace LBDatabase {

template<class EntityClassLeft, class EntityClassRight>
class ConcreteRelation : public RelationLocal
{
protected:
    friend class StorageLocalPrivate;
    friend class RelationValueRightPrivate;
    friend class EntityTypeLocalPrivate;

    explicit ConcreteRelation(Row *row, StorageLocal *parent) :
        RelationLocal(row, parent)
    {
    }

    virtual RelationValueBaseLocal *createLeftValue(EntityLocal *entity);
    virtual RelationValueBaseLocal *createRightValue(EntityLocal *entity);
};

template<class EntityClassLeft, class EntityClassRight>
RelationValueBaseLocal *ConcreteRelation<EntityClassLeft, EntityClassRight>::createLeftValue(EntityLocal *entity)
{
    return new RelationValueLocal<EntityClassRight>(this, entity);
}

template<class EntityClassLeft, class EntityClassRight>
RelationValueBaseLocal *ConcreteRelation<EntityClassLeft, EntityClassRight>::createRightValue(EntityLocal *entity)
{
    return new RelationValueLocal<EntityClassLeft>(this, entity);
}

} // namespace LBDatabase

#endif // LBDATABASE_CONCRETERELATION_H
