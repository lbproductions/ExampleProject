#ifndef LBDATABASE_RELATIONVALUE_H
#define LBDATABASE_RELATIONVALUE_H

#include "propertyvalue.h"

#include "storage.h"
#include "context.h"
#include "functionvalue.h"
#include "entity.h"

#include <QVariant>
#include <QDebug>

namespace LBDatabase {

class Relation;
class Entity;
class Attribute;

class RelationValue : public PropertyValue
{
    Q_OBJECT
public:
    RelationValue(QObject *parent) : PropertyValue(parent) {}
    ~RelationValue() {}

    virtual QList<Entity *> entitiesUncast() const = 0;
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(QList<LBDatabase::Entity *>)

#endif // LBDATABASE_RELATIONVALUE_H
