#include "dbtablemodel.h"
#include "localdatainterface.h"
#include "progressmanager.h"

DBTableModel::DBTableModel(RamObject::ObjectType type, QObject *parent):
    RamAbstractObjectModel{type, parent}
{
    m_lookUpKey = "shortName";
}

void DBTableModel::addFilterValue(QString key, QString value)
{
    QStringList filterValues = m_filters.value(key);
    filterValues << value;
    m_filters.insert(key, filterValues);
}

void DBTableModel::addFilterValues(QString key, QStringList values)
{
    QStringList filterValues = m_filters.value(key);
    filterValues.append(values);
    m_filters.insert(key, filterValues);
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

    RamAbstractObjectModel::moveObjects(sourceRow, count, destinationChild);

    // Save order
    for (int i = 0; i <= rowCount(); i++)
    {
        RamObject *o = this->get( i );
        if (o) o->setOrder(i);
    }

    endMoveRows();

    return true;
}

void DBTableModel::insertObjects(int row, QVector<QStringList> data, QString table)
{
    // Wrong table, not for us
    if (table != m_table) return;

    // Check row
    if (row < 0) row = 0;
    if (row > rowCount()) row = rowCount();

    // Insert
    beginInsertRows(QModelIndex(), row, row + data.count()-1);

    for (int i = data.count() - 1; i >= 0; i--)
    {
        QStringList obj = data.at(i);
        QString uuid = obj.first();
        QString dataStr = obj.at(1);

        RamAbstractObjectModel::insertObject(row, uuid, dataStr);
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

        beginRemoveRows(QModelIndex(), i, i);

        // Remove from underlying data
        RamAbstractObjectModel::removeObject(uuid);

        endRemoveRows();
    }
}

int DBTableModel::getOrder(QString data)
{
    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject obj = doc.object();
    return obj.value("order").toInt(-1);
}

bool DBTableModel::checkFilters(QString data) const
{
    if (m_filters.isEmpty()) return true;

    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject obj = doc.object();

    // Iterate through the filters;
    // An STL const iterator is the fastest
    QHash<QString, QStringList>::const_iterator i = m_filters.constBegin();
    while (i != m_filters.constEnd())
    {
        QString key = i.key();
        QStringList values = i.value();

        // The data must satisfy ALL the filters
        if (key != "" && !values.isEmpty())
        {
            if ( !values.contains( obj.value(key).toString() ) ) return false;
        }

        i++;
    }

    return true;
}

void DBTableModel::insertObject(QString uuid, QString data, QString table)
{
    if (table != m_table) return;

    // Filter
    if (!checkFilters(data)) return;

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
    ProgressManager *pm = ProgressManager::instance();
    pm->setText(tr("Reloading '%1'").arg(m_table));

    beginResetModel();
    // Empty
    clear();

    // Get all
    QVector<QStringList> objs = LocalDataInterface::instance()->tableData( m_table, m_filters );
    qDebug() << "Got " << objs.count() << " objects from " << m_table;
    // Sort
    std::sort(objs.begin(), objs.end(), objSorter);
    // Insert
    insertObjects(0, objs, m_table);

    endResetModel();
}

void DBTableModel::changeData(QString uuid, QString data)
{
    if (!m_objectUuids.contains(uuid)) return;

    // Check if the order has changed
    int order = getOrder(data);
    int currentOrder = m_objectUuids.indexOf(uuid);
    if (order >= 0 && order != currentOrder)
    {
        if (order > rowCount()) order = rowCount();
        moveRows(QModelIndex(), currentOrder, 1, QModelIndex(), order);
    }

    // Update stored data
    RamAbstractObjectModel::updateObject(uuid, data);

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
