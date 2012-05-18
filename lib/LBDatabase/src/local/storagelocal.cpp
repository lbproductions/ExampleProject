#include "storage.h"

#include "attributelocal.h"
#include "column.h"
#include "concreterelation.h"
#include "contextlocal.h"
#include "database.h"
#include "entitylocal.h"
#include "entitytypelocal.h"
#include "enumattributelocal.h"
#include "functionlocal.h"
#include "../propertyvalue.h"
#include "relationlocal.h"
#include "row.h"
#include "table.h"

#include <QFile>
#include <QMutex>
#include <QFileInfo>

#include <QDebug>

namespace LBDatabase {

/******************************************************************************
** StoragePrivate
*/
namespace {
const QString MetaDataTableName("lbmeta");
const QString ContextsTableName("lbmeta_contexts");
const QString EntitiesTableName("lbmeta_entitytypes");
const QString AttributesTableName("lbmeta_attributes");
const QString RelationsTableName("lbmeta_relations");
const QString FunctionsTableName("lbmeta_functions");
const QString PropertiesTableName("lbmeta_properties");
const QString DependenciesTableName("lbmeta_dependencies");

const QString NameColumn("name");
const QString SourcePathColumn("cpppath");

const QString PropertyIdColumn("id");
const QString PropertyTypeColumn("type");
const QString PropertyTypeIdColumn("typeId");

const QString DependencyIdColumn("id");
const QString DependencyPropertyColumn("property");
const QString DependencyDependsOnColumn("dependsOn");
}

class StorageLocalPrivate {
    static QHash<QString, StorageLocal*> instances;

    StorageLocalPrivate() : database(0) {}

    void init();
    bool open();
    ContextLocal *addContext(const QString &name, const QString &baseEntityTypeName);

    ContextLocal *createContextInstance(Row *row);

    Table *attributesTable;
    Table *contextsTable;
    Table *entityTypesTable;
    Table *metaDataTable;
    Table *relationsTable;
    Table *functionsTable;
    Table *propertiesTable;

    QString name;
    QString sourcePath;
    QString fileName;
    Database *database;

    QHash<int, Context *> contexts;
    QHash<QString, int> contextIds;
    QHash<int, EntityType *> entityTypes;
    QHash<int, Attribute *> attributes;
    QHash<int, Relation *> relations;
    QHash<int, Function *> functions;
    QHash<int, Property *> properties;

    QHash<QString, QMetaObject> contextMetaObjects;

    StorageLocal * q_ptr;
    Q_DECLARE_PUBLIC(StorageLocal)
};

QHash<QString, StorageLocal*> StorageLocalPrivate::instances = QHash<QString, StorageLocal*>();

void StorageLocalPrivate::init()
{
    database = Database::instance(fileName);
}

bool sortContextByName(const Context *c1, const Context *c2)
{
    return c1->identifier() < c2->identifier();
}

bool StorageLocalPrivate::open()
{
    Q_Q(StorageLocal);
    if(!database)
        return false;

    QFile file(fileName);
    if(!file.exists()) {
        file.open(QFile::ReadOnly);
        file.close();
    }

    if(!database->open())
        return false;

    metaDataTable = database->table(MetaDataTableName);
    if(!metaDataTable)
        return false;

    contextsTable = database->table(ContextsTableName);
    if(!contextsTable)
        return false;

    entityTypesTable = database->table(EntitiesTableName);
    if(!entityTypesTable)
        return false;

    attributesTable = database->table(AttributesTableName);
    if(!attributesTable)
        return false;

    relationsTable = database->table(RelationsTableName);
    if(!relationsTable)
        return false;

    functionsTable = database->table(FunctionsTableName);
    if(!functionsTable)
        return false;

    propertiesTable = database->table(PropertiesTableName);
    if(!propertiesTable)
        return false;

    Row *metaDataRow = metaDataTable->rowAt(0);
    name = metaDataRow->data(NameColumn).toString();
    sourcePath = metaDataRow->data(SourcePathColumn).toString();

    contexts.reserve(contextsTable->rows().size());
    entityTypes.reserve(entityTypesTable->rows().size());
    attributes.reserve(attributesTable->rows().size());
    properties.reserve(attributesTable->rows().size() + relationsTable->rows().size() + functionsTable->rows().size());

    foreach(Row *row, contextsTable->rows()) {
        ContextLocal *context = createContextInstance(row);
        contexts.insert(row->id(), context);
        contextIds.insert(context->identifier(), row->id());
    }

    foreach(Row *row, entityTypesTable->rows()) {
        q->insertEntityType(new EntityTypeLocal(row, q));
    }

    foreach(Row *row, attributesTable->rows()) {
        if(static_cast<AttributeLocal::Type>(row->data(AttributeLocal::TypeColumn).toInt()) == AttributeLocal::Enum) {
            q->insertAttribute(new EnumAttributeLocal(row, q));
        }
        else {
            q->insertAttribute(new AttributeLocal(row, q));
        }
    }

    foreach(Row *row, database->table(EnumAttributeLocal::EnumsTable)->rows()) {
        EnumAttributeLocal *attribute = static_cast<EnumAttributeLocal *>(attributes.value(row->data(EnumAttributeLocal::AttributeColumn).toInt()));
        attribute->addEnumValue(row->data(EnumAttributeLocal::IdentifierColumn).toString(),
                                row->data(EnumAttributeLocal::ValueColumn).toInt());
    }

    foreach(Row *row, relationsTable->rows()) {
        q->insertRelation(new RelationLocal(row, q));
    }

    foreach(Row *row, functionsTable->rows()) {
        q->insertFunction(new FunctionLocal(row, q));
    }

    foreach(Row *row, database->table(FunctionLocal::FunctionReimplementationsTable)->rows()) {
        FunctionLocal *function = static_cast<FunctionLocal *>(functions.value(row->data(FunctionLocal::ReimplementedFunctionColumn).toInt()));
        EntityTypeLocal *type = static_cast<EntityTypeLocal *>(entityTypes.value(row->data(FunctionLocal::ReimplementingEntityTypeColumn).toInt()));
        function->addReimplementingEntityType(type);
    }

    foreach(Context *c, contexts.values()) {
        ContextLocal *context = static_cast<ContextLocal *>(c);
        context->initializeEntityHierarchy();
        context->loadEntities();
    }

    properties.clear();
    foreach(Row *propertyRow, propertiesTable->rows()) {
        Property *property = 0;
        int id = propertyRow->data(PropertyIdColumn).toInt();
        int typeId = propertyRow->data(PropertyTypeIdColumn).toInt();
        switch(propertyRow->data(PropertyTypeColumn).toInt()) {
        case Property::Attribute:
        case Property::EnumAttribute:
            property = attributes.value(typeId);
            break;
        case Property::Relation:
            property = relations.value(typeId);
            break;
        case Property::Function:
            property = functions.value(typeId);
            break;
        }
        property->_storageGlobalId = id;
        properties.insert(id, property);
    }

    QList<Property *> propertiesList = properties.values();
    foreach(Property *property, propertiesList) {
        property->addPropertyValueToEntities();
    }
    foreach(Property *property, propertiesList) {
        property->fetchValues();
    }
    foreach(Row *dependencyRow, database->table(DependenciesTableName)->rows()) {
        int dependendPropertyId = dependencyRow->data(DependencyPropertyColumn).toInt();
        int dependsOnId = dependencyRow->data(DependencyDependsOnColumn).toInt();
        Property *property = properties.value(dependsOnId);
        Property *dependendProperty = properties.value(dependendPropertyId);
        property->addDependendProperty(dependendProperty);

        qDebug() << dependendProperty->entityType()->displayName() << "::" << dependendProperty->displayName() << " depends on " <<
                    property->entityType()->displayName() << "::" << property->displayName();
    }
    foreach(Property *property, propertiesList) {
        property->connectDependendPropertyValues();
    }

    return true;
}

ContextLocal *StorageLocalPrivate::addContext(const QString &name, const QString &baseEntityTypeName)
{
    database->createTable(name);
    Row *row = contextsTable->appendRow();
    row->setData(ContextLocal::IdentifierColumn, QVariant(name));

    ContextLocal *context = createContextInstance(row);
    contexts.insert(row->id(), context);

    context->createBaseEntityType(baseEntityTypeName);

    return context;
}

ContextLocal *StorageLocalPrivate::createContextInstance(Row *row)
{
    Q_Q(StorageLocal);
    const QString contextName = row->data(ContextLocal::IdentifierColumn).toString();

    if(!contextMetaObjects.contains(contextName))
        return new ContextLocal(row, q);

    QObject *object = contextMetaObjects.value(contextName).newInstance(Q_ARG(::LBDatabase::Row*,row), Q_ARG(::LBDatabase::StorageLocal*, q));
    return static_cast<ContextLocal *>(object);
}

/******************************************************************************
** Storage
*/
/*!
  \class Storage
  \brief The Storage class represents a high level storage for entities.

  \ingroup highlevel-database-classes

  \todo Dokument
  */

/*!
  \var Storage::d_ptr
  \internal
  */

/*!
  \fn Storage::nameChanged()

  This signal is emitted when the name of this storage changes.

  */

/*!
  Returns a storage instance, which holds the data contained in the storage file
  \a fileName.

  This will return exactly one instance per file, i.e. you can open each file
  only exactly once.
  */
StorageLocal *StorageLocal::instance(const QString &fileName)
{
    static QMutex mutex(QMutex::Recursive);
    static QObject guard;
    QMutexLocker locker(&mutex);
    Q_UNUSED(locker)

    if(StorageLocalPrivate::instances.contains(fileName)) {
        return StorageLocalPrivate::instances.value(fileName);
    }

    StorageLocal* storage = new StorageLocal(fileName, &guard);
    StorageLocalPrivate::instances.insert(fileName, storage);
    return storage;
}

void StorageLocal::convertSqlliteDatabaseToStorage(const QString &sqliteDatabaseFileName, const QString &storageFileName)
{
    QFile file(sqliteDatabaseFileName);
    file.open(QFile::ReadOnly);
    file.copy(storageFileName);
    file.close();


    Database *database = Database::instance(storageFileName);
    database->open();
    QList<Table *> tables = database->tables();

    database->createTable(MetaDataTableName);
    database->createTable(ContextsTableName);
    database->createTable(EntitiesTableName);
    database->createTable(AttributesTableName);
    database->createTable(RelationsTableName);
    Table *metaDataTable = database->table(MetaDataTableName);
    Table *contextsTable = database->table(ContextsTableName);
    Table *entityTypesTable = database->table(EntitiesTableName);
    Table *attributesTable = database->table(AttributesTableName);
    Table *relationsTable = database->table(RelationsTableName);

    metaDataTable->addColumn(NameColumn,QLatin1String("TEXT"));
    metaDataTable->appendRow();

    contextsTable->addColumn(ContextLocal::IdentifierColumn,QLatin1String("TEXT"));

    entityTypesTable->addColumn(EntityTypeLocal::ContextColumn,QLatin1String("INTERGER"));
    entityTypesTable->addColumn(EntityTypeLocal::IdentifierColumn,QLatin1String("TEXT"));
    entityTypesTable->addColumn(EntityTypeLocal::ParentEntityTypeColumn,QLatin1String("INTERGER"));

    attributesTable->addColumn(AttributeLocal::IdentifierColumn,QLatin1String("TEXT"));
    attributesTable->addColumn(AttributeLocal::DisplayNameColumn,QLatin1String("TEXT"));
    attributesTable->addColumn(AttributeLocal::EntityTypeIdColumn,QLatin1String("INTERGER"));

//    relationsTable->addColumn(Relation::NameColumn,QLatin1String("TEXT"));
//    relationsTable->addColumn(Relation::DisplayNameLeftColumn,QLatin1String("TEXT"));
//    relationsTable->addColumn(Relation::DisplayNameRightColumn,QLatin1String("TEXT"));
//    relationsTable->addColumn(Relation::EntityTypeLeftColumn,QLatin1String("INTERGER"));
//    relationsTable->addColumn(Relation::EntityTypeRightColumn,QLatin1String("INTERGER"));
//    relationsTable->addColumn(Relation::CardinalityColumn,QLatin1String("INTERGER"));

    StorageLocal *storage = StorageLocal::instance(storageFileName);
    storage->open();
    storage->setName(QFileInfo(storageFileName).fileName());

    foreach(Table *table, tables) {
        Context *context = storage->addContext(table->name(), table->name().remove(table->name().size() - 1, 1));
        EntityType *base = context->baseEntityType();

        table->addColumn(EntityLocal::EntityTypeIdColumn, QLatin1String("INTEGER"), QVariant(base->id()));

        foreach(Column *column, table->columns()) {
            if(column->name().compare("ID", Qt::CaseInsensitive) != 0) {
                base->addAttribute(column->name(),AttributeLocal::Unkown);
            }
        }
    }
}

/*!
  Closes the storage.
  */
StorageLocal::~StorageLocal()
{
}

/*!
  Returns the database, on which the storage works. Note that changes to the
  database are not automatically reflected in the storage.
  */
Database *StorageLocal::database() const
{
    Q_D(const StorageLocal);
    return d->database;
}

/*!
  Returns the name of the storage.
  */
QString StorageLocal::name() const
{
    Q_D(const StorageLocal);
    return d->name;
}

/*!
  Sets the name of the storage to \a name. Note that this name is not connected
  to the file name, but is stored in the database itself.
  */
void StorageLocal::setName(const QString &name)
{
    Q_D(StorageLocal);
    if(d->name == name)
        return;

    d->metaDataTable->rowAt(0)->setData(NameColumn, QVariant(name));
    d->name = name;
    emit nameChanged(name);
}

/*!
  Creates a storage, which uses the database \a fileName.
  */
StorageLocal::StorageLocal(const QString &fileName, QObject *parent) :
    Storage(parent),
    d_ptr(new StorageLocalPrivate)
{
    Q_D(StorageLocal);
    d->q_ptr = this;
    d->fileName = fileName;
    d->init();
}

/*!
  Returns the file name of the storage.
  */
QString StorageLocal::fileName() const
{
    Q_D(const StorageLocal);
    return d->fileName;
}

QString StorageLocal::sourcePath() const
{
    Q_D(const StorageLocal);
    return d->sourcePath;
}

/*!
  Returns the EntityType with the ID \a id.
  */
EntityTypeLocal *StorageLocal::entityType(int id) const
{
    Q_D(const StorageLocal);
    return static_cast<EntityTypeLocal *>(d->entityTypes.value(id, 0));
}

/*!
  Returns the EntityType with the ID \a id.
  */
Context *StorageLocal::context(int id) const
{
    Q_D(const StorageLocal);
    return d->contexts.value(id, 0);
}

Context *StorageLocal::context(const QString name) const
{
    Q_D(const StorageLocal);
    return d->contexts.value(d->contextIds.value(name));
}

/*!
  Returns a list of all contexts in this storage.
  */
QList<Context *> StorageLocal::contexts() const
{
    Q_D(const StorageLocal);
    QList<Context *> contexts = d->contexts.values();
    qSort(contexts.begin(), contexts.end(), sortContextByName);
    return contexts;
}

/*!
  Creates a new context in the storage with the name \a name. It will
  automatically create a base entity type for the context named \a
  baseEntityTypeName, since each context has to have exactly one base entity
  type.
  */
Context *StorageLocal::addContext(const QString &name, const QString &baseEntityTypeName)
{
    Q_D(StorageLocal);
    return d->addContext(name, baseEntityTypeName);
}

/*!
  Returns the Attribute with the ID \a id.
  */
AttributeLocal *StorageLocal::attribute(int id) const
{
    Q_D(const StorageLocal);
    return static_cast<AttributeLocal *>(d->attributes.value(id, 0));
}

/*!
  \internal

  Inserts the entity type \a type into the storage-global list of types.
  */
void StorageLocal::insertEntityType(EntityTypeLocal *type)
{
    Q_D(StorageLocal);
    if(d->entityTypes.contains(type->id()))
        return;

    d->entityTypes.insert(type->id(), type);
}

/*!
  \internal

  Inserts the attribute \a attribute into the storage-global list of attributes.
  */
void StorageLocal::insertAttribute(AttributeLocal *attribute)
{
    Q_D(StorageLocal);
    if(d->attributes.contains(attribute->id()))
        return;

    d->attributes.insert(attribute->id(), attribute);
    d->properties.insert(attribute->storageGlobalId(), attribute);
}

/*!
  \internal

  Inserts the relation \a relation into the storage-global list of relations.
  */
void StorageLocal::insertRelation(RelationLocal *relation)
{
    Q_D(StorageLocal);
    if(d->relations.contains(relation->id()))
        return;

    d->relations.insert(relation->id(), relation);
    d->properties.insert(relation->storageGlobalId(), relation);
}

/*!
  \internal

  Inserts the function \a function into the storage-global list of functions.
  */
void StorageLocal::insertFunction(FunctionLocal *function)
{
    Q_D(StorageLocal);
    if(d->functions.contains(function->id()))
        return;

    d->functions.insert(function->id(), function);
    d->properties.insert(function->storageGlobalId(), function);
}

QList<Relation *> StorageLocal::relations() const
{
    Q_D(const StorageLocal);
    return d->relations.values();
}

/*!
  Returns the table, which defines all contexts.
  */
Table *StorageLocal::contextsTable() const
{
    Q_D(const StorageLocal);
    return d->contextsTable;
}

/*!
  Returns the table, which defines all entity types.
  */
Table *StorageLocal::entityTypesTable() const
{
    Q_D(const StorageLocal);
    return d->entityTypesTable;
}

/*!
  Returns the table, which defines all attributes.
  */
Table *StorageLocal::attributesTable() const
{
    Q_D(const StorageLocal);
    return d->attributesTable;
}

Table *StorageLocal::propertiesTable() const
{
    Q_D(const StorageLocal);
    return d->propertiesTable;
}

void StorageLocal::registerContextType(const QString &contextName, QMetaObject metaObject)
{
    Q_D(StorageLocal);
    if(d->contextMetaObjects.contains(contextName))
        return;

    d->contextMetaObjects.insert(contextName, metaObject);
}

/*!
  Opens the storage.
  Returns true upon success and false if something goes wrong (e.g. the file is
  no correct storage).
  */
bool StorageLocal::open()
{
    Q_D(StorageLocal);
    return d->open();
}

/*!
  Returns a list of all entity types, that this storage may contain.
  */
QList<EntityType *> StorageLocal::entityTypes() const
{
    Q_D(const StorageLocal);
    return d->entityTypes.values();
}

} // namespace LBDatabase
