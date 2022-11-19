#include "dbtablemodel.h"
#include "localdatainterface.h"

DBTableModel::DBTableModel(RamObject::ObjectType type, QObject *parent):
    RamAbstractObjectModel{type, parent}
{
    m_filterKey = "";
    m_filterValues = QStringList();
    m_lookUpKey = "shortName";
}

void DBTableModel::setFilterKey(QString key)
{
    m_filterKey = key;
}

void DBTableModel::addFilterValue(QString value)
{
    m_filterValues << value;
}

void DBTableModel::setLookUpKey(const QString &newLookUpKey)
{
    m_lookUpKey = newLookUpKey;
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
    connect(ldi, &LocalDataInterface::dataChanged, this, &DBTableModel::changeData);
    connect(ldi, &LocalDataInterface::dataReset, this, &DBTableModel::reload);
}

int DBTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_objectUuids.count();
}

int DBTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 1;
}

QVariant DBTableModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();

    if (row < 0) return QVariant();
    else if (row >= rowCount()) return QVariant();

    QString uuid = m_objectUuids.at(row);
    if (role == RamObject::UUID) return uuid;

    // For other roles, we need the actual object
    RamObject *obj = RamObject::get(uuid, m_table);
    if (obj)
    {
        if (role == RamObject::Pointer) return reinterpret_cast<quintptr>(obj);
        return obj->roleData(role);
    }

    // Object does not exist
    if (role == RamObject::Pointer) return 0;
    return uuid;
}

QVariant DBTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(role);

    // No column data
    if (orientation == Qt::Horizontal) return QVariant();

    // Handle wrong section
    if (section < 0) return QVariant();
    if (section >= rowCount()) return QVariant();

    QString uuid = m_objectUuids.at(section);
    if (role == RamObject::UUID) return uuid;

    // For other roles, we need the actual object
    RamObject *obj = RamObject::get(uuid, m_table);
    if (obj)
    {
        if (role == RamObject::Pointer) return reinterpret_cast<quintptr>(obj);
        if (role == Qt::DisplayRole) return obj->shortName();
    }

    if (role == RamObject::Pointer) return 0;
    if (role == Qt::DisplayRole) return "";

    return QAbstractTableModel::headerData(section, orientation, role);
}

bool DBTableModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    Q_UNUSED(sourceParent)
    Q_UNUSED(destinationParent)

    int sourceEnd = sourceRow + count-1;
    int d = destinationChild;
    if (sourceRow < destinationChild) d++;

    if (!beginMoveRows(QModelIndex(), sourceRow, sourceEnd, QModelIndex(), d))
        return false;

    for (int i = 0; i < count ; i++)
    {
        if (destinationChild < sourceRow) {
            m_objectUuids.move(sourceEnd, destinationChild);
        }
        else {
            m_objectUuids.move(sourceRow, destinationChild);
        }
    }

    // Save order
    int movedCount = sourceEnd - sourceRow;
    int first = std::min(sourceRow, d);
    int last = std::max(sourceEnd, d + movedCount);

    for (int i = first; i <= last; i++)
    {
        RamObject *o = this->get(i);
        if (o) o->setOrder(i);
    }

    endMoveRows();

    return true;
}

RamObject *DBTableModel::search(QString searchString) const
{
    // Shortname first
    // And try with the lookup table first
    if (m_lookUpKey == "shortName")
    {
        QStringList uuids = m_lookUpTable.values(searchString);
        for(int i = 0; i < uuids.count(); i++)
        {
            QString uuid = uuids.at(i);
            if (uuid == "") continue;
            RamObject *o = RamObject::get(uuid, m_table);
            if (o) return o;
        }
    }
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name after
    if (m_lookUpKey == "name")
    {
        QStringList uuids = m_lookUpTable.values(searchString);
        for(int i = 0; i < uuids.count(); i++)
        {
            QString uuid = uuids.at(i);
            if (uuid == "") continue;
            RamObject *o = RamObject::get(uuid, m_table);
            if (o) return o;
        }
    }
    for (int i = 0; i < m_objectUuids.count(); i++)
    {
        QString uuid = m_objectUuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

QList<RamObject *> DBTableModel::lookUp(QString lookUpValue)
{
    QList<QString> uuids = m_lookUpTable.values(lookUpValue);
    QList<RamObject *> objs;
    for (int i = 0; i < uuids.count(); i++)
    {
        QString uuid = uuids.at(i);
        if (uuid == "") continue;
        RamObject *o = RamObject::get(uuid, m_table);
        if (!o) continue;
        objs << o;
    }
    return objs;
}

void DBTableModel::clear()
{
    if (rowCount() == 0) return;

    beginResetModel();

    RamAbstractObjectModel::clear();
    m_lookUpTable.clear();

    endResetModel();
}

void DBTableModel::insertObjects(int row, QVector<QStringList> data, QString table)
{
    // Wrong table, not for us
    if (table != m_table) return;

    // Sanitize
    for (int i = data.count() - 1; i >= 0; i--)
    {
        QStringList obj = data.at(i);
        QString dataStr = obj.last();

        if ( !checkFilters(dataStr) ) {
            data.removeAt(i);
            continue;
        }

        data[i][1] = getLookUpValue(dataStr);
    }

    // Nothing to insert
    if (data.count() == 0) return;

    // Check row
    if (row < 0) row = 0;
    if (row > rowCount()) row = rowCount();

    // Insert
    beginInsertRows(QModelIndex(), row, row + data.count()-1);

    for (int i = data.count() - 1; i >= 0; i--)
    {
        QStringList obj = data.at(i);
        QString uuid = obj.first();
        QString lookUpKey = obj.last();

        m_objectUuids.insert(row, uuid);
        m_lookUpTable.insert(lookUpKey, uuid);
    }

    endInsertRows();
}

void DBTableModel::removeObjects(QStringList uuids, QString table)
{
    // Not for us
    if (table != "" && table != m_table) return;

    // TODO maybe group calls to batch remove contiguous rows
    // if there are performance issues
    // beginRemoveRows can take a group of rows
    while (!uuids.isEmpty())
    {
        QString uuid = uuids.takeLast();
        int i = m_objectUuids.indexOf(uuid);
        while( i >= 0 )
        {
            beginRemoveRows(QModelIndex(), i, i);

            // Remove from uuid list
            m_objectUuids.removeAt(i);

            // Remove from lookup table
            QMutableHashIterator<QString, QString> it(m_lookUpTable);
            while (it.hasNext())
            {
                it.next();
                if (it.value() == uuid) it.remove();
            }

            i = m_objectUuids.indexOf(uuid);

            endRemoveRows();

        }
    }
}

int DBTableModel::getOrder(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject obj = doc.object();
    return obj.value("order").toInt(-1);
}

QString DBTableModel::getLookUpValue(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject obj = doc.object();
    return obj.value(m_lookUpKey).toString("default");
}

bool DBTableModel::checkFilters(QString data) const
{
    // Check data
    if (m_filterKey != "" && !m_filterValues.isEmpty())
    {
        QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
        QJsonObject obj = doc.object();
        if ( !m_filterValues.contains( obj.value(m_filterKey).toString() ) ) return false;
    }
    return true;
}

void DBTableModel::insertObject(QString uuid, QString data, QString table)
{
    if (table != m_table) return;

    // Check order
    int order = getOrder(data);

    // Insert
    QStringList o;
    o << uuid << data;
    insertObjects( order, QVector<QStringList>() << o, table );
}

void DBTableModel::removeObject(QString uuid, QString table)
{
    // Remove
    removeObjects( QStringList(uuid), table );
}

void DBTableModel::reload()
{
    // Empty
    clear();

    // Get all
    QVector<QStringList> objs = LocalDataInterface::instance()->tableData( m_table );
    // Sort
    std::sort(objs.begin(), objs.end(), objSorter);
    // Insert
    insertObjects(0, objs, m_table);
}

void DBTableModel::changeData(QString uuid, QString data)
{
    if (!m_objectUuids.contains(uuid)) return;

    // Check filters
    if (!checkFilters(data)) removeObject(uuid, m_table);

    // Check if the order has changed
    int order = getOrder(data);
    int currentOrder = m_objectUuids.indexOf(uuid);
    if (order >= 0 && order != currentOrder) moveRows(QModelIndex(), currentOrder, 1, QModelIndex(), order);

    // Emit data changed
    QModelIndex i = index( m_objectUuids.indexOf(uuid), 1);
    emit dataChanged(i, i);
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
