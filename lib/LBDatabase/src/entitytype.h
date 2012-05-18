#ifndef LBDATABASE_ENTITYTYPE_H
#define LBDATABASE_ENTITYTYPE_H

#include <QObject>

#include "attribute.h"
#include "relation.h"

namespace LBDatabase {

class Attribute;
class Calculator;
class Context;
class Entity;
class Function;
class Property;
class Row;
class Storage;

class EntityType : public QObject
{
    Q_OBJECT
public:
    EntityType(QObject *parent) : QObject(parent) {}

    virtual int id() const = 0;
    virtual QString identifier() const = 0;
    virtual void setIdentifier(const QString &identifier) = 0;
    virtual QString displayName() const = 0;
    virtual void setDisplayName(const QString &displayName) = 0;
    virtual QString displayNamePlural() const = 0;
    virtual void setDisplayNamePlural(const QString &displayNamePlural) = 0;
    virtual Context *context() const = 0;
    virtual EntityType *parentEntityType() const = 0;
    virtual int parentEntityTypeId() const = 0;

    virtual QList<EntityType *> childEntityTypes() const = 0;

    virtual Property *property(const QString &identifier) const = 0;

    virtual QList<Property *> properties() const = 0;
    virtual QList<Attribute *> attributes() const = 0;
    virtual QList<Relation *> relations() const = 0;
    virtual QList<Function *> functions() const = 0;

    virtual QList<Property *> nonInhertitedProperties() const = 0;
    virtual QList<Attribute *> nonInhertitedAttributes() const = 0;
    virtual QList<Relation *> nonInhertitedRelations() const = 0;
    virtual QList<Function *> nonInhertitedFunctions() const = 0;

    virtual Attribute *addAttribute(const QString &identifier, Attribute::Type type) = 0;
    virtual Relation *addRelation(const QString &identifier, EntityType *otherType, Relation::Cardinality cardinality) = 0;

    virtual QList<Entity *> entities() const = 0;

    virtual bool inherits(EntityType *entityType) const = 0;

    virtual Calculator *calculator() const = 0;

Q_SIGNALS:
    void identifierChanged(QString identifier);
    void displayNameChanged(QString displayName);

private:
    friend class EntityTypeLocalPrivate;
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITYTYPE_H
