#ifndef LBDATABASE_PROPERTY_H
#define LBDATABASE_PROPERTY_H

#include <QObject>

namespace LBDatabase {

class Context;
class Entity;
class EntityType;
class StoragePrivate;
class ContextPrivate;

//! \cond PRIVATE
class Property : public QObject
{
    Q_OBJECT
public:
    explicit Property(QObject *parent = 0);

    enum Type {
        Attribute = 1,
        EnumAttribute,
        Relation,
        Function
    };

    int storageGlobalId() const;
    virtual int id() const = 0;
    virtual EntityType *entityType() const = 0;
    virtual QString identifier() const = 0;
    virtual QString displayName() const = 0;
    virtual void setDisplayName(const QString &displayName) = 0;

    virtual Type propertyType() const = 0;

Q_SIGNALS:
    void displayNameChanged(QString displayName);

private:
    friend class StorageLocalPrivate;
    friend class ContextLocalPrivate;

    virtual void addPropertyValueToEntities() = 0;
    virtual void addPropertyValue(Entity *entity) = 0;
    virtual void addDependendProperty(Property *property) = 0;
    virtual void connectDependendPropertyValues() = 0;

    virtual void fetchValues() = 0;

    int _storageGlobalId;
};
//! \endcond

} // namespace LBDatabase

#endif // LBDATABASE_PROPERTY_H
