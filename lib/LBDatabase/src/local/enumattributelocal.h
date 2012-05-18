#ifndef LBDATABASE_ENUMATTRIBUTELOCAL_H
#define LBDATABASE_ENUMATTRIBUTELOCAL_H

#include "attributelocal.h"

#include <QMap>

namespace LBDatabase {

class EnumAttributeLocalPrivate;
class EnumAttributeLocal : public AttributeLocal
{
    Q_OBJECT
public:
    static const QString EnumsTable;
    static const QString AttributeColumn;
    static const QString IdentifierColumn;
    static const QString ValueColumn;

    QString stringValue(int value) const;

    QString typeName() const;
    QString qtType() const;

    QMap<int, QString> enumValues() const;

    Property::Type propertyType() const;

private:
    friend class StorageLocalPrivate;
    EnumAttributeLocal(Row *row, StorageLocal *parent);

    void addEnumValue(const QString &identifier, int value);

    Q_DECLARE_PRIVATE(EnumAttributeLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENUMATTRIBUTELOCAL_H
