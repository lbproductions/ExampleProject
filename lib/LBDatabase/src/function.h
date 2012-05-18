#ifndef LBDATABASE_FUNCTION_H
#define LBDATABASE_FUNCTION_H

#include "property.h"

namespace LBDatabase {

class EntityType;
class Storage;

class Function : public Property
{
public:
    Function(QObject *parent) : Property(parent) {}

    virtual EntityType *keyEntityType() const = 0;
    virtual QString qtTypeName() const = 0;

    virtual bool isCalculated() const = 0;
    virtual bool cacheData() const = 0;
    virtual bool isEditable() const = 0;

    virtual QList<EntityType *> reimplementingEntityTypes() const = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_FUNCTION_H
