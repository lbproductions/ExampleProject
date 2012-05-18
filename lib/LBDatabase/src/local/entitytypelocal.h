#ifndef LBDATABASE_ENTITYTYPELOCAL_H
#define LBDATABASE_ENTITYTYPELOCAL_H

#include "../entitytype.h"

#include "attributelocal.h"
#include "relationlocal.h"

namespace LBDatabase {

class AttributeLocal;
class Calculator;
class ContextLocal;
class EntityLocal;
class FunctionLocal;
class Property;
class Row;
class StorageLocal;

class EntityTypeLocalPrivate;
class EntityTypeLocal : public EntityType
{
    Q_OBJECT
public:
    //! \cond PRIVATE
    static const QString ContextColumn;
    static const QString IdentifierColumn;
    static const QString ParentEntityTypeColumn;
    static const QString DisplayNameColumn;
    static const QString DisplayNamePluralColumn;
    //! \endcond

    ~EntityTypeLocal();

    int id() const;
    QString identifier() const;
    void setIdentifier(const QString &identifier);
    QString displayName() const;
    void setDisplayName(const QString &displayName);
    QString displayNamePlural() const;
    void setDisplayNamePlural(const QString &displayNamePlural);
    Context *context() const;
    EntityType *parentEntityType() const;
    int parentEntityTypeId() const;

    QList<EntityType *> childEntityTypes() const;

    Property *property(const QString &identifier) const;

    QList<Property *> properties() const;
    QList<Attribute *> attributes() const;
    QList<Relation *> relations() const;
    QList<Function *> functions() const;

    QList<Property *> nonInhertitedProperties() const;
    QList<Attribute *> nonInhertitedAttributes() const;
    QList<Relation *> nonInhertitedRelations() const;
    QList<Function *> nonInhertitedFunctions() const;

    Attribute *addAttribute(const QString &identifier, Attribute::Type type);
    Relation *addRelation(const QString &identifier, EntityType *otherType, Relation::Cardinality cardinality);

    QList<Entity *> entities() const;

    bool inherits(EntityType *entityType) const;

    Calculator *calculator() const;

Q_SIGNALS:
    void identifierChanged(QString identifier);
    void displayNameChanged(QString displayName);

private:
    friend class ContextLocalPrivate;
    friend class StorageLocalPrivate;
    friend class AttributeLocalPrivate;
    friend class RelationLocalPrivate;
    friend class EntityLocalPrivate;
    friend class FunctionLocalPrivate;
    friend class TransposeRelationPrivate;

    explicit EntityTypeLocal(LBDatabase::Row *row, StorageLocal *parent);

    void setContext(ContextLocal *context);
    void addChildEntityType(EntityTypeLocal *type);
    void setParentEntityType(EntityTypeLocal *type);
    void addAttribute(AttributeLocal *attribute);
    void addRelation(RelationLocal *relation);
    void addFunction(FunctionLocal *function);
    void inheritProperties(EntityTypeLocal *parent);
    void inheritCalculator(EntityTypeLocal *parent);
    void addEntity(EntityLocal *entity);
    void setCalculator(Calculator *calculator);

    QScopedPointer<EntityTypeLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(EntityTypeLocal)
    Q_DISABLE_COPY(EntityTypeLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_ENTITYTYPELOCAL_H
