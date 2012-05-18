#ifndef LBDATABASE_ATTRIBUTEVALUE_H
#define LBDATABASE_ATTRIBUTEVALUE_H

#include "propertyvalue.h"

namespace LBDatabase {

class AttributeValue : public PropertyValue
{
public:
    AttributeValue(QObject *parent) : PropertyValue(parent) {}
    ~AttributeValue() {}
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTEVALUE_H
