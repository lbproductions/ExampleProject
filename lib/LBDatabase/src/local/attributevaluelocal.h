#ifndef LBDATABASE_ATTRIBUTEVALUELOCAL_H
#define LBDATABASE_ATTRIBUTEVALUELOCAL_H

#include "../attributevalue.h"

namespace LBDatabase {

class AttributeLocal;
class EntityLocal;

class AttributeValueLocalPrivate;
class AttributeValueLocal : public AttributeValue
{
    Q_OBJECT
public:
    ~AttributeValueLocal();

    Entity *entity() const;
    Property *property() const;
    QVariant data(int role = Qt::DisplayRole) const;
    bool setData(const QVariant &data);

    bool isEditable() const;

private:
    friend class AttributeLocalPrivate;

    explicit AttributeValueLocal(AttributeLocal *attribute, EntityLocal *parent);

    void fetchValue();
    void calculate();

    QScopedPointer<AttributeValueLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(AttributeValueLocal)
    Q_DISABLE_COPY(AttributeValueLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_ATTRIBUTEVALUELOCAL_H
