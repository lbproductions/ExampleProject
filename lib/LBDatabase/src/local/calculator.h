#ifndef LBDATABASE_CALCULATOR_H
#define LBDATABASE_CALCULATOR_H

#include <QObject>

#include <QHash>

namespace LBDatabase {

class AttributeValueLocal;
class Entity;
class FunctionValueLocal;


class Calculator : public QObject
{
    Q_OBJECT
public:
    explicit Calculator(QObject *parent = 0);
    
    QVariant calculate(const Entity *entity, AttributeValueLocal *attributeValue);

    QHash<const LBDatabase::Entity *, QVariant> calculate(const Entity *entity, FunctionValueLocal *functionValue);
    QVariant calculate(const Entity *entity, FunctionValueLocal *functionValue, const Entity *key);
};

} // namespace LBDatabase

typedef QHash<const LBDatabase::Entity *, QVariant> EntityVariantHash;

#endif // LBDATABASE_CALCULATOR_H
