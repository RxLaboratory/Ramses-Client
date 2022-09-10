#include "dbtablemodel.h"
#include "localdatainterface.h"

DBTableModel::DBTableModel(RamAbstractObject::ObjectType type, QObject *parent):
    RamObjectModel(type, parent)
{
    // Initial loading
    reload();

    // Monitor the DB for changes
    LocalDataInterface *ldi = LocalDataInterface::instance();
    connect(ldi, &LocalDataInterface::inserted, this, &DBTableModel::insertObject);
    connect(ldi, &LocalDataInterface::removed, this, &DBTableModel::removeObject);
    connect(ldi, &LocalDataInterface::dataReset, this, &DBTableModel::reload);
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

void DBTableModel::reload()
{
    clear();
    QStringList uuids = LocalDataInterface::instance()->tableUuids( RamObject::objectTypeName( type() ));
    insertObjects(0, uuids);
}
