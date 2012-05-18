#ifndef LBDATABASE_RELATIONVALUELOCAL_P_H
#define LBDATABASE_RELATIONVALUELOCAL_P_H

#include <QObject>

#include <QHash>

namespace LBDatabase {

class Entity;
class EntityLocal;
class FunctionValueLocal;
class RelationLocal;
class RelationValueBaseLocal;

//! \cond PRIVATE
class RelationValueBaseLocalPrivate {
protected:
    friend class RelationPrivate;

    RelationValueBaseLocalPrivate() {}

    virtual void init();
    virtual void fetchValue();

    EntityLocal *entity;
    RelationLocal *relation;

    RelationValueBaseLocal * q_ptr;
    Q_DECLARE_PUBLIC(RelationValueBaseLocal)
};
//! \endcond

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONVALUELOCAL_P_H
