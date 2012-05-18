#ifndef LBDATABASE_CONTEXTLOCAL_H
#define LBDATABASE_CONTEXTLOCAL_H

#include <QAbstractTableModel>
#include "../context.h"

#include <QMetaType>

namespace LBDatabase {

class AttributeLocal;
class EntityLocal;
class EntityTypeLocal;
class FunctionLocal;
class RelationLocal;
class Row;
class StorageLocal;
class Table;

class ContextLocalPrivate;
class ContextLocal : public Context
{
    Q_OBJECT
public:
    //! \cond PRIVATE
    static const QString IdentifierColumn;
    static const QString DisplayNameColumn;
    static const QString TableNameColumn;
    //! \endcond

    ~ContextLocal();

    int id() const;
    QString identifier() const;
    QString displayName() const;
    Storage *storage() const;
    Table *table() const;

    EntityType *baseEntityType() const;
    QList<EntityType *> entityTypes() const;
    EntityType *addEntityType(const QString &identifier, EntityType *parentEntityType);

    Entity *entity(int id) const;
    QList<Entity *> entities() const;
    Entity *insertEntity(EntityType *type);

    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int columnCount(const QModelIndex &parent) const;
    int rowCount(const QModelIndex &parent) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags(const QModelIndex &index) const;


private Q_SLOTS:
    void onEntityTypeNameChanged(QString identifier);
    void onPropertyDisplayNameChanged(QString displayName);
    void onPropertyValueDataChanged(QVariant data);

protected:
    friend class StorageLocalPrivate;
    friend class EntityTypeLocalPrivate;
    friend class AttributeLocalPrivate;
    friend class RelationLocalPrivate;
    friend class FunctionLocalPrivate;
    friend class TransposeRelationPrivate;

    explicit ContextLocal(Row *row, StorageLocal *parent);

    template<class EntityClass>
    void registerEntityClass();

    template<class EntityClass, class CalculatorClass>
    void registerCalculatorClass();

private:
    void createBaseEntityType(const QString &identifier);
    void addEntityType(EntityTypeLocal *type);
    void addAttribute(AttributeLocal *attribute);
    void addFunction(FunctionLocal *function);
    void addRelation(RelationLocal *relation);

    void initializeEntityHierarchy();
    void loadEntities();

    void registerEntityClass(const QString &entityTypeName, QMetaObject metaObject);
    void registerCalculatorClass(const QString &entityTypeName, QMetaObject metaObject);

    QScopedPointer<ContextLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(ContextLocal)
    Q_DISABLE_COPY(ContextLocal)
};

template<class EntityClass>
void ContextLocal::registerEntityClass()
{
    registerEntityClass(EntityClass::Name, EntityClass::staticMetaObject);
}

template<class EntityClass, class CalculatorClass>
void ContextLocal::registerCalculatorClass()
{
    registerCalculatorClass(EntityClass::Name, CalculatorClass::staticMetaObject);
}

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::ContextLocal*)

#endif // LBDATABASE_CONTEXTLOCAL_H
