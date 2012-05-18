#include "functionvaluelocal.h"

#include "calculator.h"
#include "contextlocal.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "functionlocal.h"
#include "row.h"
#include "table.h"

#include <QDebug>

#define COMMA ,
Q_DECLARE_METATYPE(QHash<const LBDatabase::Entity * COMMA QVariant>)

namespace LBDatabase {
/******************************************************************************
** FunctionValuePrivate
*/
class FunctionValueLocalPrivate {
    FunctionValueLocalPrivate() : cached(false) {}

    void init();
    void fetchValue();
    QHash<const Entity *, QVariant> calculate();
    QVariant calculate(const Entity *key);

    Entity *entity;
    FunctionLocal *function;

    mutable bool cached;

    mutable QHash<const Entity *, QVariant> values;
    mutable QHash<const Entity *, Row *> valueRows;

    FunctionValueLocal * q_ptr;
    Q_DECLARE_PUBLIC(FunctionValueLocal)
};

void FunctionValueLocalPrivate::init()
{
    Q_Q(FunctionValueLocal);
    QObject::connect(q, SIGNAL(dataChanged(QVariant)), entity->context(), SLOT(onPropertyValueDataChanged(QVariant)));
}

void FunctionValueLocalPrivate::fetchValue()
{
}

QVariant FunctionValueLocalPrivate::calculate(const Entity *key)
{
    Q_Q(FunctionValueLocal);
    Calculator *calculator = entity->entityType()->calculator();

    if(calculator)
        return calculator->calculate(entity,q,key);

    return QVariant();
}

QHash<const Entity *, QVariant> FunctionValueLocalPrivate::calculate()
{
    Q_Q(FunctionValueLocal);
    Calculator *calculator = entity->entityType()->calculator();

    if(calculator)
        return calculator->calculate(entity,q);

    return QHash<const Entity *, QVariant>();
}

/******************************************************************************
** FunctionValue
*/
FunctionValueLocal::FunctionValueLocal(FunctionLocal *function, Entity *parent) :
    FunctionValue(parent),
    d_ptr(new FunctionValueLocalPrivate)
{
    Q_D(FunctionValueLocal);
    d->q_ptr = this;
    d->function = function;
    d->entity = parent;
    d->init();
}

void FunctionValueLocal::fetchValue()
{
    Q_D(FunctionValueLocal);
    d->fetchValue();
}

void FunctionValueLocal::calculate()
{
}

void FunctionValueLocal::addValue(Entity *key, const QVariant &value, Row *row)
{
    Q_D(FunctionValueLocal);
    d->values.insert(key, value);
    d->valueRows.insert(key, row);
}

FunctionValueLocal::~FunctionValueLocal()
{
}

Entity *FunctionValueLocal::entity() const
{
    Q_D(const FunctionValueLocal);
    return d->entity;
}

Property *FunctionValueLocal::property() const
{
    Q_D(const FunctionValueLocal);
    return d->function;
}

QVariant FunctionValueLocal::value(const Entity *entity) const
{
    Q_D(const FunctionValueLocal);

    if(d->function->isCalculated()) {
        if(!d->function->cacheData()) {
            return const_cast<FunctionValueLocalPrivate*>(d)->calculate(static_cast<const EntityLocal *>(entity));
        }
        if(!d->cached) {
            d->values = const_cast<FunctionValueLocalPrivate*>(d)->calculate();
            d->cached = true;
        }
    }

    return d->values.value(static_cast<const EntityLocal *>(entity));
}

QVariant FunctionValueLocal::data(int role) const
{
    Q_D(const FunctionValueLocal);
    Q_UNUSED(role);

    if(d->function->isCalculated()) {
        if(!d->function->cacheData()) {
            return QVariant::fromValue<QHash<const Entity *, QVariant> >(const_cast<FunctionValueLocalPrivate*>(d)->calculate());
        }
        if(!d->cached) {
            d->values = const_cast<FunctionValueLocalPrivate*>(d)->calculate();
            d->cached = true;
        }
    }

    if(role == Qt::DisplayRole) {
        if(d->values.isEmpty())
            return QVariant();

        if(d->values.size() == 1) {
            const Entity *key = d->values.keys().at(0);
            QVariant value = d->values.value(key);
            return QVariant(key->displayName() + QLatin1String("=") + value.toString());
        }

        return QVariant(QString::number(d->values.size()) +QLatin1String(" values"));
    }
    else if(role == PropertyValue::PlainDataRole) {
        return QVariant::fromValue<QHash<const Entity *, QVariant> >(d->values);
    }

    return QVariant();
}

bool FunctionValueLocal::setData(const QVariant &data)
{
    return false;
}

QHash<const Entity *, QVariant> FunctionValueLocal::values() const
{
    Q_D(const FunctionValueLocal);

    return d->values;
}

void FunctionValueLocal::setValue(const Entity *key, const QVariant &value)
{
    Q_D(const FunctionValueLocal);
    if(d->function->isCalculated())
        return;

    if(d->values.value(key) == value)
        return;

    if(d->valueRows.contains(key)) {
        d->valueRows.value(key)->setData(d->function->valueColumnName(), value);
    }
    else {
        Row *row = d->function->functionTable()->appendRow();
        row->setData(d->function->entityColumnName(), d->entity->id());
        row->setData(d->function->keyEntityColumnName(), key->id());
        row->setData(d->function->valueColumnName(), value);
        d->valueRows.insert(key, row);
    }
    d->values.insert(key, value);
    emit valueChanged(key, value);
}

bool FunctionValueLocal::isEditable() const
{
    return false;
}

} // namespace LBDatabase
