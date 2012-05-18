#ifndef LBDATABASE_FUNCTIONLOCAL_H
#define LBDATABASE_FUNCTIONLOCAL_H

#include "../function.h"

namespace LBDatabase {

class EntityTypeLocal;
class Row;
class StorageLocal;
class Table;

class FunctionLocalPrivate;
class FunctionLocal : public Function
{
public:
    static const QString IdentifierColumn;
    static const QString DisplayNameColumn;
    static const QString TableNameColumn;
    static const QString EntityColumnNameColumn;
    static const QString KeyEntityColumnNameColumn;
    static const QString ValueColumnNameColumn;
    static const QString EntityTypeColumn;
    static const QString KeyEntityTypeRightColumn;
    static const QString CalculatedColumn;
    static const QString CacheDataColumn;
    static const QString TypeColumn;
    static const QString EditableColumn;


    static const QString FunctionReimplementationsTable;
    static const QString ReimplementedFunctionColumn;
    static const QString ReimplementingEntityTypeColumn;

    ~FunctionLocal();

    int id() const;
    EntityType *entityType() const;
    QString displayName() const;
    void setDisplayName(const QString &displayName);
    QString identifier() const;

    EntityType *keyEntityType() const;
    QString qtTypeName() const;

    bool isCalculated() const;
    bool cacheData() const;
    bool isEditable() const;

    QList<EntityType *> reimplementingEntityTypes() const;

    Property::Type propertyType() const;

private:
    friend class StorageLocalPrivate;
    friend class FunctionValueLocal;

    explicit FunctionLocal(Row *row, StorageLocal *parent);

    void addReimplementingEntityType(EntityTypeLocal *type);

    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);
    void fetchValues();
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    Table *functionTable() const;
    QString entityColumnName() const;
    QString keyEntityColumnName() const;
    QString valueColumnName() const;

    QScopedPointer<FunctionLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(FunctionLocal)
    Q_DISABLE_COPY(FunctionLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_FUNCTIONLOCAL_H
