#ifndef LBDATABASE_STORAGE_H
#define LBDATABASE_STORAGE_H

#include <QObject>

#include <QMetaType>

namespace LBDatabase {

class Attribute;
class Context;
class EntityType;
class Function;
class Relation;

class StoragePrivate;
class Storage : public QObject
{
    Q_OBJECT
public:
    Storage(QObject *parent) : QObject(parent) {}

    virtual bool open() = 0;

    virtual QString name() const = 0;
    virtual void setName(const QString &name) = 0;

    virtual Context *context(const QString name) const = 0;

    virtual QList<Context *> contexts() const = 0;
    virtual QList<EntityType *> entityTypes() const = 0;
    virtual QList<Relation *> relations() const = 0;

    virtual Context *addContext(const QString &name, const QString &baseEntityTypeName) = 0;

Q_SIGNALS:
    void nameChanged(QString name);
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Storage *)

#endif // LBDATABASE_STORAGE_H
