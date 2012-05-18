#ifndef ATTRIBUTELOCAL_P_H
#define ATTRIBUTELOCAL_P_H

#include "attributelocal.h"

namespace LBDatabase {

class AttributeLocalPrivate {
protected:
    AttributeLocalPrivate() {}

    virtual void init();
    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);
    void fetchValues();
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    Row *row;
    StorageLocal *storage;
    QString identifier;
    QString displayName;
    EntityTypeLocal *entityType;
    bool calculated;
    bool cacheData;
    bool editable;
    QList<Property *> dependendProperties;

    AttributeLocal::Type type;

    int columnIndex;

    AttributeLocal * q_ptr;
    Q_DECLARE_PUBLIC(AttributeLocal)
};

} // namespace LBDatabase

#endif // ATTRIBUTELOCAL_P_H
