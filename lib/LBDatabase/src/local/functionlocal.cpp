#include "functionlocal.h"

#include "attributelocal.h"
#include "column.h"
#include "contextlocal.h"
#include "database.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "functionvaluelocal.h"
#include "row.h"
#include "storagelocal.h"
#include "table.h"

#include <QDebug>
#include <QThread>

namespace LBDatabase {

/******************************************************************************
** FunctionPrivate
*/
const QString FunctionLocal::IdentifierColumn("identifier");
const QString FunctionLocal::TableNameColumn("tableName");
const QString FunctionLocal::EntityColumnNameColumn("entityColumnName");
const QString FunctionLocal::KeyEntityColumnNameColumn("keyEntityColumnName");
const QString FunctionLocal::ValueColumnNameColumn("valueColumnName");
const QString FunctionLocal::DisplayNameColumn("displayName");
const QString FunctionLocal::EntityTypeColumn("entityType");
const QString FunctionLocal::KeyEntityTypeRightColumn("keyEntityType");
const QString FunctionLocal::CalculatedColumn("calculated");
const QString FunctionLocal::CacheDataColumn("cacheData");
const QString FunctionLocal::TypeColumn("type");
const QString FunctionLocal::EditableColumn("editable");

const QString FunctionLocal::FunctionReimplementationsTable("lbmeta_functionreimplementations");
const QString FunctionLocal::ReimplementedFunctionColumn("function");
const QString FunctionLocal::ReimplementingEntityTypeColumn("reimplementingEntityType");

class FunctionLocalPrivate {
    FunctionLocalPrivate() :
        entityType(0),
        keyEntityType(0),
        functionTable(0)
    {}

    void init();
    void addPropertyValueToEntities();
    void addPropertyValue(EntityLocal *entity);
    void fetchValues();
    void addDependendProperty(Property *property);
    void connectDependendPropertyValues();

    void setValue(EntityLocal *key, const QVariant &value);

    Row *row;
    StorageLocal *storage;
    EntityTypeLocal *entityType;
    EntityTypeLocal *keyEntityType;
    AttributeLocal::Type type;

    bool calculated;
    bool cacheData;
    bool editable;
    QString identifier;
    QString displayName;
    QString tableName;
    QString entityColumnName;
    QString keyEntityColumnName;
    QString valueColumnName;

    Table *functionTable;

    QList<EntityType *> reimplementingEntityTypes;
    QList<Property *> dependendProperties;

    FunctionLocal * q_ptr;
    Q_DECLARE_PUBLIC(FunctionLocal)
};

void FunctionLocalPrivate::init()
{
    Q_Q(FunctionLocal);
    identifier = row->data(FunctionLocal::IdentifierColumn).toString();
    displayName = row->data(FunctionLocal::DisplayNameColumn).toString();
    entityType = storage->entityType(row->data(FunctionLocal::EntityTypeColumn).toInt());
    keyEntityType = storage->entityType(row->data(FunctionLocal::KeyEntityTypeRightColumn).toInt());
    calculated = row->data(FunctionLocal::CalculatedColumn).toBool();
    cacheData = row->data(FunctionLocal::CacheDataColumn).toBool();
    editable = row->data(FunctionLocal::EditableColumn).toBool();
    type = static_cast<AttributeLocal::Type>(row->data(FunctionLocal::TypeColumn).toInt());

    tableName = row->data(FunctionLocal::TableNameColumn).toString();
    entityColumnName = row->data(FunctionLocal::EntityColumnNameColumn).toString();
    keyEntityColumnName = row->data(FunctionLocal::KeyEntityColumnNameColumn).toString();
    valueColumnName = row->data(FunctionLocal::ValueColumnNameColumn).toString();

    if(!calculated) {
        functionTable = storage->database()->table(tableName);

        if(!functionTable) {
            qWarning() << "No such table:" << identifier << "for function" << row->id();
        }
    }

    entityType->addFunction(q);
    static_cast<ContextLocal *>(entityType->context())->addFunction(q);
}

void FunctionLocalPrivate::addPropertyValueToEntities()
{
    Q_Q(FunctionLocal);
    foreach(Entity *entity, entityType->entities()) {
        static_cast<EntityLocal *>(entity)->addFunctionValue(new FunctionValueLocal(q, static_cast<EntityLocal *>(entity)));
    }
}

void FunctionLocalPrivate::addPropertyValue(EntityLocal *entity)
{
    Q_Q(FunctionLocal);
    entity->addFunctionValue(new FunctionValueLocal(q, entity));
}

void FunctionLocalPrivate::fetchValues()
{
    Q_Q(FunctionLocal);
    if(!calculated) {
        int entityColumn = functionTable->column(entityColumnName)->index();
        int keyEntityColumn = functionTable->column(keyEntityColumnName)->index();
        int valueColumn = functionTable->column(valueColumnName)->index();
        Entity *entity;
        Entity *keyEntity;
        QVariant value;
        FunctionValueLocal *functionValue;
        foreach(Row *row, functionTable->rows()) {
            entity = entityType->context()->entity(row->data(entityColumn).toInt());
            keyEntity = keyEntityType->context()->entity(row->data(keyEntityColumn).toInt());
            value = row->data(valueColumn);
            if(!entity || !keyEntity)
                continue;

            functionValue = static_cast<FunctionValueLocal *>(entity->propertyValue(q));
            functionValue->addValue(static_cast<EntityLocal *>(keyEntity), value, row);
        }
    }
}

void FunctionLocalPrivate::addDependendProperty(Property *property)
{
    dependendProperties.append(property);
}

void FunctionLocalPrivate::connectDependendPropertyValues()
{
    Q_Q(FunctionLocal);
    foreach(Property *property, dependendProperties) {
        foreach(Entity *dependendEntity, property->entityType()->entities()) {
            foreach(Entity *entity, q->entityType()->entities()) {
                QObject::connect(entity->propertyValue(q), SIGNAL(dataChanged(QVariant)),
                        dependendEntity->propertyValue(property), SLOT(calculate()));
            }
        }
    }
}

/******************************************************************************
** Function
*/
FunctionLocal::FunctionLocal(Row *row, StorageLocal *parent) :
    Function(parent),
    d_ptr(new FunctionLocalPrivate)
{
    Q_D(FunctionLocal);
    d->q_ptr = this;
    d->row = row;
    d->storage = parent;
    d->init();
}

void FunctionLocal::addReimplementingEntityType(EntityTypeLocal *type)
{
    Q_D(FunctionLocal);
    d->reimplementingEntityTypes.append(type);
}

FunctionLocal::~FunctionLocal()
{
}

int FunctionLocal::id() const
{
    Q_D(const FunctionLocal);
    return d->row->id();
}

EntityType *FunctionLocal::entityType() const
{
    Q_D(const FunctionLocal);
    return d->entityType;
}

Property::Type FunctionLocal::propertyType() const
{
    return Property::Function;
}

Table *FunctionLocal::functionTable() const
{
    Q_D(const FunctionLocal);
    return d->functionTable;
}

QString FunctionLocal::entityColumnName() const
{
    Q_D(const FunctionLocal);
    return d->entityColumnName;
}

QString FunctionLocal::keyEntityColumnName() const
{
    Q_D(const FunctionLocal);
    return d->keyEntityColumnName;
}

QString FunctionLocal::valueColumnName() const
{
    Q_D(const FunctionLocal);
    return d->valueColumnName;
}

QString FunctionLocal::displayName() const
{
    Q_D(const FunctionLocal);
    return d->displayName;
}

void FunctionLocal::setDisplayName(const QString &displayName)
{
    Q_D(FunctionLocal);
    if(d->displayName == displayName)
        return;

    d->row->setData(AttributeLocal::DisplayNameColumn, QVariant(displayName));
    d->displayName = displayName;
    emit displayNameChanged(displayName);
}

QString FunctionLocal::identifier() const
{
    Q_D(const FunctionLocal);
    return d->identifier;
}

EntityType *FunctionLocal::keyEntityType() const
{
    Q_D(const FunctionLocal);
    return d->keyEntityType;
}

QString FunctionLocal::qtTypeName() const
{
    Q_D(const FunctionLocal);
    return AttributeLocal::typeToQtType(d->type);
}

bool FunctionLocal::isCalculated() const
{
    Q_D(const FunctionLocal);
    return d->calculated;
}

bool FunctionLocal::cacheData() const
{
    Q_D(const FunctionLocal);
    return d->cacheData;
}

bool FunctionLocal::isEditable() const
{
    Q_D(const FunctionLocal);
    return d->editable;
}

QList<EntityType *> FunctionLocal::reimplementingEntityTypes() const
{
    Q_D(const FunctionLocal);
    return d->reimplementingEntityTypes;
}

void FunctionLocal::addPropertyValueToEntities()
{
    Q_D(FunctionLocal);
    d->addPropertyValueToEntities();
}

void FunctionLocal::addPropertyValue(Entity *entity)
{
    Q_D(FunctionLocal);
    d->addPropertyValue(static_cast<EntityLocal *>(entity));
}

void FunctionLocal::fetchValues()
{
    Q_D(FunctionLocal);
    d->fetchValues();
}

void FunctionLocal::addDependendProperty(Property *property)
{
    Q_D(FunctionLocal);
    d->addDependendProperty(property);
}

void FunctionLocal::connectDependendPropertyValues()
{
    Q_D(FunctionLocal);
    d->connectDependendPropertyValues();
}

} // namespace LBDatabase
