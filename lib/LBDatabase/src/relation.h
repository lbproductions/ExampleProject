#ifndef LBDATABASE_RELATION_H
#define LBDATABASE_RELATION_H

#include "property.h"

namespace LBDatabase {

class Context;
class EntityType;
class RelationValue;
class Row;
class Storage;
class Table;

class Relation : public Property
{
    Q_OBJECT
public:
    enum Cardinality {
        OneToOne,
        OneToMany,
        ManyToMany
    };

    enum Direction {
        LeftToRight,
        Both
    };

    Relation(QObject *parent) : Property(parent) {}
    ~Relation() {}
    virtual EntityType *entityTypeOther() const = 0;
    virtual Cardinality cardinality() const = 0;
    virtual Direction direction() const = 0;

    virtual bool isEditable() const = 0;
    virtual bool isTranspose() const = 0;

    virtual Storage* storage() const = 0;
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATION_H
