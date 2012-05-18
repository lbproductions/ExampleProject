#ifndef LBDATABASE_ENTITY_H
#define LBDATABASE_ENTITY_H

#include <QObject>

#include "entitytype.h"

#include <QHash>

namespace LBDatabase {

class Attribute;
class AttributeValue;
class Context;
class FunctionValue;
class Property;
class PropertyValue;
class Relation;
class RelationValue;
class Row;
class Storage;

class Entity : public QObject
{
    Q_OBJECT
public:
    Entity(QObject *parent) : QObject(parent) {}
    ~Entity() {}

    virtual int id() const = 0;
    virtual QString displayName() const = 0;

    virtual EntityType *entityType() const = 0;
    virtual Storage *storage() const = 0;
    virtual Context *context() const = 0;
    virtual QList<PropertyValue *> propertyValues() const = 0;
    virtual PropertyValue *propertyValue(Property *property) const = 0;
    virtual QVariant value(const QString &name) const = 0;
    virtual void setValue(const QString &name, const QVariant &data) = 0;

    virtual RelationValue *relationUncast(const QString &name) const = 0;

    virtual FunctionValue *function(const QString &name) const = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITY_H
