#include "enumattributelocal.h"

#include "attributelocal_p.h"
#include "database.h"
#include "entitytypelocal.h"
#include "storagelocal.h"

#include <QStringList>

namespace LBDatabase {

const QString EnumAttributeLocal::EnumsTable("lbmeta_enums");
const QString EnumAttributeLocal::AttributeColumn("attribute");
const QString EnumAttributeLocal::IdentifierColumn("name");
const QString EnumAttributeLocal::ValueColumn("value");


class EnumAttributeLocalPrivate : public AttributeLocalPrivate {
    void init();

    QMap<int, QString> enumValues;

    Q_DECLARE_PUBLIC(EnumAttributeLocal)
};

void EnumAttributeLocalPrivate::init()
{
    AttributeLocalPrivate::init();
}

EnumAttributeLocal::EnumAttributeLocal(Row *row, StorageLocal *parent) :
    AttributeLocal(*new EnumAttributeLocalPrivate, row, parent)
{
}

Property::Type EnumAttributeLocal::propertyType() const
{
    return Property::EnumAttribute;
}

QString EnumAttributeLocal::stringValue(int value) const
{
    Q_D(const EnumAttributeLocal);
    return d->enumValues.value(value);
}

QString EnumAttributeLocal::typeName() const
{
    Q_D(const EnumAttributeLocal);
    return d->identifier;
}

QString EnumAttributeLocal::qtType() const
{
    return typeName().left(1).toUpper() + typeName().mid(1);
}

QMap<int, QString> EnumAttributeLocal::enumValues() const
{
    Q_D(const EnumAttributeLocal);
    return d->enumValues;
}

void EnumAttributeLocal::addEnumValue(const QString &name, int value)
{
    Q_D(EnumAttributeLocal);
    d->enumValues.insert(value, name);
}

} // namespace LBDatabase
