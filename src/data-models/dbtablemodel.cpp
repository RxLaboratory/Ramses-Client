#include "dbtablemodel.h"
#include "localdatainterface.h"

DBTableModel::DBTableModel(RamAbstractObject::ObjectType type, QObject *parent):
    RamObjectModel(type, parent)
{
    LocalDataInterface *ldi = LocalDataInterface::instance();

    // Initial loading
    QStringList uuids = ldi->tableUuids( RamObject::objectTypeName( type ));
    insertObjects(0, uuids);

    // Monitor the DB for changes
    connect(ldi, &LocalDataInterface::inserted, this, &DBTableModel::insertObject);
    connect(ldi, &LocalDataInterface::removed, this, &DBTableModel::removeObject);
}

void DBTableModel::insertObject(QString uuid, QString table)
{
    RamObject::ObjectType t = RamObject::objectTypeFromName( table );
    // Not for us
    if (t != m_type) return;

    // Insert
    insertObjects( rowCount(), QStringList(uuid));
}

void DBTableModel::removeObject(QString uuid, QString table)
{
    RamObject::ObjectType t = RamObject::objectTypeFromName( table );
    // Not for us
    if (t != m_type) return;

    // Remove
    removeObjects( QStringList(uuid) );
}
