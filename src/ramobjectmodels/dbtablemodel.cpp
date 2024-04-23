#include "dbtablemodel.h"
#include "duqf-app/app-config.h"
#include "localdatainterface.h"
#include "dbinterface.h"
#include "progressmanager.h"

DBTableModel::DBTableModel(RamObject::ObjectType type, bool projectTable, bool sorted, QObject *parent):
    RamAbstractObjectModel{type, parent}
{
    m_isProjectTable = projectTable;
    m_userOrder = sorted;
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

void DBTableModel::setRejectInvalidData(bool r)
{
    m_rejectInvalidData = r;
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
    if (m_isProjectTable) connect(ldi, &LocalDataInterface::dataResetProject, this, &DBTableModel::reload);
    else connect(ldi, &LocalDataInterface::dataResetCommon, this, &DBTableModel::reload);
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

    endMoveRows();

    saveOrder();

    return true;
}

void DBTableModel::insertObjects(int row, const QVector<QStringList> &data, const QString &table, bool silent)
{
    // Wrong table, not for us
    if (table != "" && table != m_table) return;

    // Check row
    if (row < 0) row = 0;
    if (row > rowCount()) row = rowCount();

    // Insert
    if (!silent) beginInsertRows(QModelIndex(), row, row + data.count()-1);

    //qDebug() << "Inserting " << data.count() << " objects in " << table;

    for (int i = data.count() - 1; i >= 0; i--)
    {
        QStringList obj = data.at(i);
        QString uuid = obj.first();
        QString dataStr = obj.at(1);
        RamAbstractObjectModel::insertObject(row, uuid, dataStr);
    }

    if (!silent) endInsertRows();
}

void DBTableModel::removeObjects(QStringList uuids, const QString &table)
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

bool DBTableModel::validate(const QString &data, const QString &table)
{
    return RamObject::validateData(
        data,
        RamAbstractObject::objectTypeFromName(table)
        );
}

QString DBTableModel::checkUnique(const QString &uuid, const QString &data, const QString &modifiedDate)
{
    // Nothing to check
    if (m_uniqueDataKeys.isEmpty())
        return "";

    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject dataObj = doc.object();

    for (int i = 0; i < count(); i++) {
        RamObject *obj = get(i);
        if (!obj)
            continue;
        QJsonObject otherData = obj->data();
        bool ok = false;
        for(const QString &key: qAsConst(m_uniqueDataKeys)) {
            // If the key can't be found, consider it ok
            if (!otherData.contains(key) || !dataObj.contains(key)) {
                ok = true;
                break;
            }
            if (dataObj.value(key) == otherData.value(key))
                continue;
            ok = true;
            break;
        }
        if (!ok) {
            // Keep the most recent
            if (QDateTime::fromString(modifiedDate, DATETIME_DATA_FORMAT) >= obj->modificationDate())
                return obj->uuid();
            return uuid;
        }
    }

    return "";
}

void DBTableModel::saveOrder() const
{
    if (!m_userOrder) return;
    // Save order
    for (int i = 0; i < rowCount(); i++)
    {
        RamObject *o = this->get( i );
        if (o) o->setOrder(i);
    }
}

void DBTableModel::moveObject(int from, int to)
{
    int d = to;
    if (from < to) d++;

    if (!beginMoveRows(QModelIndex(), from, from, QModelIndex(), d))
        return;

    RamAbstractObjectModel::moveObjects(from, 1, to);

    endMoveRows();
}

void DBTableModel::insertObject(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table)
{
    if (table != m_table) return;

    // Already have it
    if (m_objectUuids.contains(uuid)) return;

    // Removed
    if (DBInterface::instance()->isRemoved(uuid, table)) return;

    // Unique
    QString uuidToRemove = checkUnique(uuid, data, modificationDate);
    if (uuidToRemove == uuid) {
        LocalDataInterface::instance()->removeObject(uuid, table);
        return;
    }

    // Validate
    if (m_rejectInvalidData && !validate(data, table))
        return;

    // Filter
    if (!checkFilters(data)) return;

    // Check order
    int order = m_objectUuids.count();
    if (m_userOrder) order = getOrder(data);

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
    if (!m_isLoaded) return;

    ProgressManager *pm = ProgressManager::instance();
    pm->setText(tr("Reloading '%1'").arg(m_table));

    beginResetModel();
    // Empty
    clear();

    // Get all
    QVector<QStringList> objs = LocalDataInterface::instance()->tableData( m_table, m_filters );

    qDebug() << "Got " << objs.count() << " objects from " << m_table;

    // Validate and uniqueness
    if (m_rejectInvalidData || !m_uniqueDataKeys.isEmpty()) {
        for (int i = objs.count() -1; i >= 0; --i) {
            QStringList data = objs.at(i);
            QString uuid = data.at(0);
            QString objData = data.at(1);
            QString modified = data.at(2);

            // Unique
            QString uuidToRemove = checkUnique(uuid, objData, modified);
            if (uuidToRemove == uuid) {
                LocalDataInterface::instance()->removeObject(uuid, m_table);
                objs.removeAt(i);
                continue;
            }

            // Validate
            if (m_rejectInvalidData && !validate(objData, m_table)) {
                objs.removeAt(i);
                continue;
            }
        }
    }

    qDebug() << "Got " << objs.count() << " filtered objects from " << m_table;

    // Sort
    if (m_userOrder) {
        std::sort(objs.begin(), objs.end(), objSorter);
        qDebug() << "Objects sorted";
    } else {
        qDebug() << "Table not sorted";
    }

    // Insert
    insertObjects(0, objs, m_table, true);

    endResetModel();
}

void DBTableModel::changeData(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table)
{
    // Not for us
    if (table != "" && table != m_table) return;

    if (!m_objectUuids.contains(uuid))
    {
        // This may be a new object to insert according to the filters
        if (!checkFilters(data)) return;
        insertObject(uuid, data, modificationDate, table);
        return;
    }

    // Check if the order has changed
    int order = getOrder(data);
    int currentOrder = m_objectUuids.indexOf(uuid);
    if (order >= 0 && order != currentOrder)
    {
        if (order >= rowCount()) order = rowCount()-1;
        moveObject(currentOrder, order);
    }

    // Update stored data
    RamAbstractObjectModel::updateObject(uuid, data);

    // Emit data changed
    QModelIndex i = index( m_objectUuids.indexOf(uuid), 0);
    emit dataChanged(i, i, QVector<int>());
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
