#ifndef LBDATABASE_RELATIONVALUELOCAL_H
#define LBDATABASE_RELATIONVALUELOCAL_H

#include "../relationvalue.h"

#include "storagelocal.h"
#include "contextlocal.h"
#include "functionvaluelocal.h"
#include "entitylocal.h"

#include <QVariant>
#include <QDebug>

namespace LBDatabase {

class RelationLocal;
class EntityLocal;
class AttributeLocal;

class RelationValueBaseLocalPrivate;
class RelationValueBaseLocal : public RelationValue
{
    Q_OBJECT
public:
    ~RelationValueBaseLocal();

    virtual Entity *entity() const;
    Property *property() const;

    bool isEditable() const;
    virtual bool setData(const QVariant &data);
    QVariant data(int role = Qt::DisplayRole) const = 0;


protected:
    friend class RelationLocalPrivate;

    explicit RelationValueBaseLocal(RelationLocal *relation, EntityLocal *parent);

    void fetchValue();
    void calculate();
    virtual void addOtherEntity(Entity *entity) = 0;

    QScopedPointer<RelationValueBaseLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(RelationValueBaseLocal)
    Q_DISABLE_COPY(RelationValueBaseLocal)
};

template<class EntityClass, class SortType>
class FunctionSorter;

enum SortingDirection {
    SortAscending,
    SortDescending
};

template<class EntityClass>
class RelationValueLocal : public RelationValueBaseLocal
{
public:
    template<typename SortType>
    QList<EntityClass *> sort(FunctionValue *sortFunction, SortingDirection dir = SortAscending)
    {
        if(otherEntities.contains(sortFunction))
            return otherEntities.value(sortFunction);


        QList<EntityClass *> list = otherEntities.value(0);

        qSort(list.begin(), list.end(), FunctionSorter<EntityClass, SortType>(sortFunction, dir));

        otherEntities.insert(sortFunction, list);
        return list;
    }

    template<typename SortType>
    QList<EntityClass *> sort(const QString &sortAttributeName, SortingDirection dir = SortAscending)
    {
        if(otherEntitiesSortedByAttribute.contains(sortAttributeName))
            return otherEntitiesSortedByAttribute.value(sortAttributeName);

        QList<EntityClass *> list = otherEntities.value(0);

        qSort(list.begin(), list.end(), FunctionSorter<EntityClass, SortType>(sortAttributeName, dir));

        otherEntitiesSortedByAttribute.insert(sortAttributeName, list);
        return list;
    }

    /*!
      Returns a QVariant, which represents the content of the relation in the given role.

      Currently this method supports only the Qt::DisplayRole.
      */
    QVariant data(int role) const
    {
        if(role == Qt::DisplayRole) {
            if(otherEntities.value(0).isEmpty())
                return QVariant();

            if(otherEntities.value(0).size() == 1)
                return static_cast<EntityLocal *>(otherEntities.value(0).at(0))->displayName();

            return QVariant(QString::number(entities().size())+QLatin1String(" ")+otherEntities.value(0).at(0)->entityType()->displayNamePlural());
        }
        else if(role == PropertyValue::PlainDataRole) {
            return QVariant::fromValue<QList<EntityLocal *> >(otherEntities.value(0));
        }

        return QVariant();
    }

    RelationValueLocal(RelationLocal *relation, EntityLocal *parent) :
        RelationValueBaseLocal(relation, parent)
    {
    }

    virtual QList<Entity *> entitiesUncast() const
    {
        return _entities;
    }

    QList<EntityClass *> entities() const
    {
        return otherEntities.value(0);
    }

    EntityClass *firstEntity() const
    {
        if(otherEntities.value(0).isEmpty())
            return 0;

        return otherEntities.value(0).first();
    }

private:
    void addOtherEntity(Entity *entity)
    {
        QList<EntityClass *> entities = otherEntities.value(0);
        EntityClass *entityCasted = static_cast<EntityClass *>(entity);
        if(!entities.contains(entityCasted)) {
            entities.append(entityCasted);
            otherEntities.insert(0, entities);
            _entities.append(entity);
        }
    }

    QList<Entity *> _entities;
    QHash<FunctionValue *, QList<EntityClass *> > otherEntities;
    QHash<QString, QList<EntityClass *> > otherEntitiesSortedByAttribute;
};

template<class EntityClass, class SortType>
class FunctionSorter
{
public:

    FunctionSorter(FunctionValue *value, SortingDirection dir) : m_sortFunction(value),m_sortAttributeName(QString()), m_dir(dir) {}
    FunctionSorter(const QString &attributeName, SortingDirection dir) : m_sortFunction(0), m_sortAttributeName(attributeName), m_dir(dir) {}

    bool operator()(const EntityClass *left, const EntityClass *right) const
    {
        QVariant leftVariant;
        QVariant rightVariant;
        if(m_sortFunction) {
            leftVariant = m_sortFunction->value(left);
            rightVariant = m_sortFunction->value(right);
        }
        else if(!m_sortAttributeName.isEmpty()) {
            leftVariant = left->value(m_sortAttributeName);
            rightVariant = right->value(m_sortAttributeName);
        }

        SortType leftValue = leftVariant.value<SortType>();
        SortType rightValue = rightVariant.value<SortType>();
        if(m_dir == SortAscending) {
            return rightValue > leftValue;
        }
        else {
            return rightValue < leftValue;
        }
    }

    FunctionValue *m_sortFunction;
    QString m_sortAttributeName;
    SortingDirection m_dir;
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(QList<LBDatabase::EntityLocal *>)

#endif // LBDATABASE_RELATIONVALUELOCAL_H
