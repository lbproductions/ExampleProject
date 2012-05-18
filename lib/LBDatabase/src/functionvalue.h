#ifndef LBDATABASE_FUNCTIONVALUE_H
#define LBDATABASE_FUNCTIONVALUE_H

#include "propertyvalue.h"

namespace LBDatabase {

class Function;

class FunctionValue : public PropertyValue
{
    Q_OBJECT
public:
    FunctionValue(QObject *parent) : PropertyValue(parent) {}

    virtual QVariant value(const Entity *entity) const = 0;
    virtual QHash<const Entity *, QVariant> values() const = 0;

    virtual void setValue(const Entity *key, const QVariant &value) = 0;

Q_SIGNALS:
    void valueChanged(const Entity *key, QVariant value);
};

} // namespace LBDatabase

#endif // LBDATABASE_FUNCTIONVALUE_H
