#ifndef LBDATABASE_ENTITYLOCAL_H
#define LBDATABASE_ENTITYLOCAL_H

#include "../entity.h"

#include "entitytypelocal.h"

#include <QHash>

namespace LBDatabase {

class AttributeLocal;
class AttributeValueLocal;
class ContextLocal;
class FunctionValueLocal;
class Property;
class PropertyValue;
class RelationLocal;
template<class EntityClass>
class RelationValueLocal;
class RelationValueBaseLocal;
class Row;
class StorageLocal;

class EntityLocalPrivate;
class EntityLocal : public Entity
{
    Q_OBJECT
public:
    //! \cond PRIVATE
    static const QString EntityTypeIdColumn;
    //! \endcond

    ~EntityLocal();

    virtual QString displayName() const;

    int id() const;
    EntityType *entityType() const;
    Storage *storage() const;
    Context *context() const;
    QList<PropertyValue *> propertyValues() const;
    PropertyValue *propertyValue(Property *property) const;
    QVariant value(const QString &name) const;
    void setValue(const QString &name, const QVariant &data);

    Row *row() const;
    virtual RelationValue *relationUncast(const QString &name) const;

    template<class EntityClass>
    RelationValueLocal<EntityClass> *relation(const QString &name) const
    {
        return static_cast<LBDatabase::RelationValueLocal<EntityClass> *>(propertyValue(entityType()->property(name)));
    }

    FunctionValue *function(const QString &name) const;

protected:
    friend class AttributeLocalPrivate;
    friend class ContextLocalPrivate;
    friend class RelationLocalPrivate;
    friend class ContextLocal;
    friend class FunctionLocalPrivate;

    explicit EntityLocal(Row *row, Context *parent);

    QVariant data(Property *property) const;
    bool setData(const QVariant &data, Property *property);

    void addAttributeValue(AttributeValueLocal *value);
    void addRelationValue(RelationValueBaseLocal *value);
    void addFunctionValue(FunctionValueLocal *value);

    QScopedPointer<EntityLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(EntityLocal)
    Q_DISABLE_COPY(EntityLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITYLOCAL_H
