#ifndef LBDATABASE_RELATIONLOCAL_H
#define LBDATABASE_RELATIONLOCAL_H

#include "../relation.h"

namespace LBDatabase {

class ContextLocal;
class EntityLocal;
class EntityTypeLocal;
class RelationValueBaseLocal;
class Row;
class StorageLocal;
class Table;

class RelationLocalPrivate;
class RelationLocal : public Relation
{
    Q_OBJECT
public:
    //! \cond PRIVATE
    static const QString IdentifierColumn;
    static const QString IdentifierRightColumn;
    static const QString DisplayNameLeftColumn;
    static const QString DisplayNameRightColumn;
    static const QString EntityTypeLeftColumn;
    static const QString EntityTypeRightColumn;
    static const QString CardinalityColumn;
    static const QString TableNameColumn;
    static const QString ColumnNameColumn;
    static const QString ColumnNameRightColumn;
    static const QString EditableColumn;
    static const QString DirectionColumn;
    //! \endcond

    ~RelationLocal();

    int id() const;
    QString displayName() const;
    void setDisplayName(const QString &displayName);

    QString identifier() const;
    EntityType *entityType() const;
    EntityType *entityTypeOther() const;
    Cardinality cardinality() const;
    Direction direction() const;

    bool isEditable() const;
    bool isTranspose() const;

    Storage* storage() const;
    Row *row() const;

    Property::Type propertyType() const;

protected:
    friend class StorageLocalPrivate;
    friend class RelationValueRightPrivate;
    friend class EntityTypePrivate;

    explicit RelationLocal(Row *row, StorageLocal *parent);
    explicit RelationLocal(RelationLocalPrivate &dd, Row *row, StorageLocal *parent);

    void addPropertyValueToEntities();
    void addPropertyValue(Entity *entity);
    void fetchValues();
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    QScopedPointer<RelationLocalPrivate> d_ptr;
    Q_DECLARE_PRIVATE(RelationLocal)
    Q_DISABLE_COPY(RelationLocal)
};

class TransposeRelation : public RelationLocal
{
    Q_OBJECT
public:
    ~TransposeRelation();

private:
    friend class RelationLocal;

    TransposeRelation(RelationLocal *relation);

    Q_DECLARE_PRIVATE(RelationLocal)
};

} // namespace LBDatabase

#endif // LBDATABASE_RELATIONLOCAL_H
