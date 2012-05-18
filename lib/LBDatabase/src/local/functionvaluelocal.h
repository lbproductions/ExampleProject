#ifndef LBDATABASE_FUNCTIONVALUELOCAL_H
#define LBDATABASE_FUNCTIONVALUELOCAL_H

#include "../functionvalue.h"

namespace LBDatabase {

class FunctionLocal;
class Row;

class FunctionValueLocalPrivate;
class FunctionValueLocal : public FunctionValue
{
    Q_OBJECT
public:
    ~FunctionValueLocal();

    Entity *entity() const;
    Property *property() const;
    QVariant value(const Entity *entity) const;
    QVariant data(int role = Qt::DisplayRole) const;
    bool setData(const QVariant &data);
    QHash<const Entity *, QVariant> values() const;

    void setValue(const Entity *key, const QVariant &value);

    bool isEditable() const;

private:
    friend class FunctionLocalPrivate;

    explicit FunctionValueLocal(FunctionLocal *function, Entity *parent);

    void fetchValue();
    void calculate();

    void addValue(Entity *key, const QVariant &value, Row *row);

    QScopedPointer<FunctionValueLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(FunctionValueLocal)
    Q_DISABLE_COPY(FunctionValueLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_FUNCTIONVALUELOCAL_H
