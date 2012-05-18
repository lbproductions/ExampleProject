#ifndef LBDATABASE_PROPERTYVALUE_H
#define LBDATABASE_PROPERTYVALUE_H

#include <QObject>

#include <QVariant>

namespace LBDatabase {

class Entity;
class StoragePrivate;
class ContextPrivate;
class EntityTypePrivate;
class RelationPrivate;
class Property;

//! \cond PRIVATE
class PropertyValue : public QObject
{
    Q_OBJECT
public:
    enum DataRole {
        PlainDataRole = Qt::UserRole + 1
    };

    explicit PropertyValue(QObject *parent = 0);

    virtual Entity *entity() const = 0;
    virtual Property *property() const = 0;
    virtual QVariant data(int role = Qt::DisplayRole) const = 0;
    virtual bool setData(const QVariant &data) = 0;

    virtual bool isEditable() const = 0;

Q_SIGNALS:
    void dataChanged(QVariant data);

protected Q_SLOTS:
    virtual void calculate() = 0;

protected:
    friend class StorageLocalPrivate;
    friend class ContextLocalPrivate;
    friend class EntityTypeLocalPrivate;
    friend class RelationLocalPrivate;

    virtual void fetchValue() = 0;
};
//! \endcond

} // namespace LBDatabase

#endif // LBDATABASE_PROPERTYVALUE_H