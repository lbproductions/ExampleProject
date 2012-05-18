#ifndef LBDATABASE_CONTEXT_H
#define LBDATABASE_CONTEXT_H

#include <QAbstractTableModel>

namespace LBDatabase {

class Entity;
class EntityType;
class Storage;

class Context : public QAbstractTableModel
{
    Q_OBJECT
public:
    Context(QObject *parent) : QAbstractTableModel(parent) {}

    virtual int id() const = 0;
    virtual QString identifier() const = 0;
    virtual QString displayName() const = 0;
    virtual Storage *storage() const = 0;

    virtual EntityType *baseEntityType() const = 0;
    virtual QList<EntityType *> entityTypes() const = 0;
    virtual EntityType *addEntityType(const QString &identifier, EntityType *parentEntityType) = 0;

    virtual Entity *entity(int id) const = 0;
    virtual QList<Entity *> entities() const = 0;
    virtual Entity *insertEntity(EntityType *type) = 0;


    // QAbstractTableModel
    QVariant data(const QModelIndex &index, int role) const = 0;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const = 0;
    int columnCount(const QModelIndex &parent) const = 0;
    int rowCount(const QModelIndex &parent) const = 0;
    bool setData(const QModelIndex &index, const QVariant &value, int role) = 0;
    Qt::ItemFlags flags(const QModelIndex &index) const = 0;
};

} // namespace LBDatabase

Q_DECLARE_METATYPE(LBDatabase::Context *)

#endif // LBDATABASE_CONTEXT_H
