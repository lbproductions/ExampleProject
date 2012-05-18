#ifndef LBDATABASE_STORAGELOCAL_H
#define LBDATABASE_STORAGELOCAL_H

#include "../storage.h"

#include <QMetaType>

namespace LBDatabase {

class AttributeLocal;
class ContextLocal;
class Database;
class EntityTypeLocal;
class FunctionLocal;
class RelationLocal;
class Table;

class StorageLocalPrivate;
class StorageLocal : public Storage
{
    Q_OBJECT
public:
    static StorageLocal *instance(const QString &fileName);

    static void convertSqlliteDatabaseToStorage(const QString &sqliteDatabaseFileName, const QString &storageFileName);

    ~StorageLocal();

    bool open();

    Database *database() const;
    QString name() const;
    void setName(const QString &name);
    QString fileName() const;
    QString sourcePath() const;

    Context *context(const QString name) const;

    QList<Context *> contexts() const;
    QList<EntityType *> entityTypes() const;

    Context *addContext(const QString &name, const QString &baseEntityTypeName);

Q_SIGNALS:
    void nameChanged(QString name);

protected:
    explicit StorageLocal(const QString &fileName, QObject *parent = 0);

    template<class ContextClass>
    void registerContextType();

    Context *context(int id) const;
    LBDatabase::EntityTypeLocal *entityType(int id) const;
    AttributeLocal *attribute(int id) const;

private:
    friend class EntityTypeLocalPrivate;
    friend class ContextLocalPrivate;
    friend class EntityLocalPrivate;
    friend class AttributeLocalPrivate;
    friend class RelationLocalPrivate;
    friend class FunctionLocalPrivate;
    friend class GraphvizExporter;
    friend class TransposeRelationPrivate;

    void insertEntityType(EntityTypeLocal *type);
    void insertAttribute(AttributeLocal *attribute);
    void insertRelation(RelationLocal *relation);
    void insertFunction(FunctionLocal *function);

    QList<Relation *> relations() const;

    Table *contextsTable() const;
    Table *entityTypesTable() const;
    Table *attributesTable() const;
    Table *propertiesTable() const;

    void registerContextType(const QString &contextName, QMetaObject metaObject);

    QScopedPointer<StorageLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(StorageLocal)
    Q_DISABLE_COPY(StorageLocal)
};

template<class ContextClass>
void StorageLocal::registerContextType()
{
    registerContextType(ContextClass::Name, ContextClass::staticMetaObject);
}

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::StorageLocal*)

#endif // LBDATABASE_STORAGELOCAL_H
