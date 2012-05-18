#ifndef LBDATABASE_ATTRIBUTELOCAL_H
#define LBDATABASE_ATTRIBUTELOCAL_H

#include "../attribute.h"

namespace LBDatabase {

class EntityLocal;
class EntityTypeLocal;
class Row;
class StorageLocal;

class AttributeLocalPrivate;
class AttributeLocal : public Attribute
{
    Q_OBJECT
public:
    static const QString IdentifierColumn;
    static const QString DisplayNameColumn;
    static const QString EntityTypeIdColumn;
    static const QString CalculatedColumn;
    static const QString CacheDataColumn;
    static const QString TypeColumn;
    static const QString EditableColumn;

    ~AttributeLocal();

    int id() const;
    EntityType *entityType() const;
    QString displayName() const;
    virtual void setDisplayName(const QString &displayName);
    int columnIndex() const;

    QString identifier() const;

    bool isCalculated() const;
    bool cacheData() const;
    bool isEditable() const;

    Property::Type propertyType() const;

    Type type() const;
    virtual QString typeName() const;
    static QString typeToName(Type type);
    static QStringList typeNames();

    virtual QString qtType() const;
    static QStringList qtTypeNames();
    static QString typeToQtType(Type type);

protected:
    friend class StorageLocalPrivate;
    friend class EntityTypeLocalPrivate;

    explicit AttributeLocal(Row *row, StorageLocal *parent);
    explicit AttributeLocal(AttributeLocalPrivate &dd, Row *row, StorageLocal *parent);

    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    void fetchValues();

    QScopedPointer<AttributeLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AttributeLocal)
    Q_DISABLE_COPY(AttributeLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTELOCAL_H
