#include "dbtablemodel.h"
#include "localdatainterface.h"

DBTableModel::DBTableModel(RamAbstractObject::ObjectType type, QObject *parent):
    RamObjectModel(type, parent)
{
    m_filterKey = "";
    m_filterValues = QStringList();
}

void DBTableModel::setFilterKey(QString key)
{
    m_filterKey = key;
}

void DBTableModel::addFilterValue(QString value)
{
    m_filterValues << value;
}

void DBTableModel::load()
{
    if (m_isLoaded) return;
    m_isLoaded = true;

    reload();

    // Monitor the DB for changes
    LocalDataInterface *ldi = LocalDataInterface::instance();
    connect(ldi, &LocalDataInterface::inserted, this, &DBTableModel::insertObject);
    connect(ldi, &LocalDataInterface::removed, this, &DBTableModel::removeObject);
    connect(ldi, &LocalDataInterface::dataReset, this, &DBTableModel::reload);

    // Monitor order changes to save the new orders
    connect(this, &RamObjectModel::rowsMoved, this, &DBTableModel::saveOrder);
}

bool DBTableModel::checkType(QString table)
{
    RamObject::ObjectType t = RamObject::objectTypeFromName( table );
    return t == m_type;
}

bool DBTableModel::checkFilters(QString uuid, QString table)
{
    if (m_filterKey != "" && !m_filterValues.isEmpty())
    {
        QString dataStr = LocalDataInterface::instance()->objectData(uuid, table);
        QJsonDocument doc = QJsonDocument::fromJson( dataStr.toUtf8() );
        QJsonObject obj = doc.object();
        if ( !m_filterValues.contains( obj.value(m_filterKey).toString() ) ) return false;
    }
    return true;
}

void DBTableModel::insertObject(QString uuid, QString table)
{
    // Not for us
    if (!checkType(table)) return;

    // Check filters
    if (!checkFilters(uuid, table)) return;

    // Insert
    insertObjects( rowCount(), QVector<QString>() << uuid );
}

void DBTableModel::removeObject(QString uuid, QString table)
{
    // Not for us
    if (!checkType(table)) return;

    // Remove
    removeObjects( QStringList(uuid) );
}

void DBTableModel::reload()
{
    clear();
    QVector<QStringList> objs = LocalDataInterface::instance()->tableData( RamObject::objectTypeName( type() ), m_filterKey, m_filterValues );

    // Sort
    std::sort(objs.begin(), objs.end(), objSorter);

    QVector<QString> uuids;
    for (int i = 0; i < objs.count(); i++)
    {
        uuids << objs[i][0];
    }

    insertObjects(0, uuids);
}

void DBTableModel::saveOrder(const QModelIndex &parent, int start, int end, const QModelIndex &destination, int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(destination);

    // Modified rows only
    int movedCount = end - start;
    int first = std::min(start, row);
    int last = std::max(end, row + movedCount);

    for (int i = first; i <= last; i++)
    {
        RamObject *o = this->get(i);
        if (o) o->setOrder(i);
    }
}

bool objSorter(const QStringList a, const QStringList b)
{
    QString dataA = a[1];
    QJsonDocument docA = QJsonDocument::fromJson(dataA.toUtf8());
    QJsonObject objA = docA.object();

    QString dataB = b[1];
    QJsonDocument docB = QJsonDocument::fromJson(dataB.toUtf8());
    QJsonObject objB = docB.object();

    int orderA = objA.value("order").toInt();
    int orderB = objB.value("order").toInt();

    if (orderA != orderB) return orderA < orderB;

    QString shortNameA = objA.value("shortName").toString();
    QString shortNameB = objB.value("shortName").toString();

    if (shortNameA != shortNameB) return shortNameA < shortNameB;

    QString nameA = objA.value("name").toString();
    QString nameB = objB.value("name").toString();

    if (nameA != nameB) return nameA < nameB;

    return false;
}
