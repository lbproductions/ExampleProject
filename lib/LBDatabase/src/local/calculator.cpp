#include "calculator.h"

#include "attributevaluelocal.h"
#include "entitylocal.h"
#include "functionvaluelocal.h"
#include "property.h"

#include <QMetaMethod>

#include <QDebug>

namespace LBDatabase {

Calculator::Calculator(QObject *parent) :
    QObject(parent)
{
}

QVariant Calculator::calculate(const Entity *entity, AttributeValueLocal *attributeValue)
{
    QString methodName = attributeValue->property()->identifier() + QLatin1String("(const LBDatabase::Entity*)");

    int methodIndex = metaObject()->indexOfMethod(methodName.toLocal8Bit().data());
    if(methodIndex == -1)
        return QVariant();

    QMetaMethod method = metaObject()->method(methodIndex);
    QVariant result;
    method.invoke(this, Q_RETURN_ARG(QVariant, result), Q_ARG(const ::LBDatabase::Entity*, entity));
    return result;
}

QHash<const Entity *, QVariant> Calculator::calculate(const Entity *entity, FunctionValueLocal *functionValue)
{
    QString methodName = functionValue->property()->identifier() + QLatin1String("(const LBDatabase::Entity*)");

    int methodIndex = metaObject()->indexOfMethod(methodName.toLocal8Bit().data());
    if(methodIndex == -1)
        return QHash<const Entity *, QVariant>();

    QMetaMethod method = metaObject()->method(methodIndex);
    EntityVariantHash result;

    method.invoke(this, Q_RETURN_ARG(EntityVariantHash, result), Q_ARG(const ::LBDatabase::Entity*, entity));
    return result;
}

QVariant Calculator::calculate(const Entity *entity, FunctionValueLocal *functionValue, const Entity *key)
{
    QString methodName = functionValue->property()->identifier() + QLatin1String("Value(const LBDatabase::Entity*,const LBDatabase::Entity*)");

    int methodIndex = metaObject()->indexOfMethod(methodName.toLocal8Bit().data());
    if(methodIndex == -1)
        return QVariant();

    QMetaMethod method = metaObject()->method(methodIndex);
    QVariant result;

    method.invoke(this, Q_RETURN_ARG(QVariant, result), Q_ARG(const ::LBDatabase::Entity*, entity), Q_ARG(const ::LBDatabase::Entity*, key));
    return result;
}

} // namespace LBDatabase
