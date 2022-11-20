#include "dbtablefilterproxymodel.h"

DBTableFilterProxyModel::DBTableFilterProxyModel(DBTableModel *model, QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_tableModel = model;
    this->setSourceModel(model);
}

void DBTableFilterProxyModel::addFilterValue(QString key, QString value)
{
    QStringList filterValues = m_filters.value(key);
    filterValues << value;
    m_filters.insert(key, filterValues);
}

void DBTableFilterProxyModel::addFilterValues(QString key, QStringList values)
{
    QStringList filterValues = m_filters.value(key);
    filterValues.append(values);
    m_filters.insert(key, filterValues);
}

void DBTableFilterProxyModel::load()
{
    m_tableModel->load();
}

bool DBTableFilterProxyModel::moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild)
{
    int sourceEnd = sourceRow + count - 1;

    // We need to move them one at a time to convert the indices
    for (int i = 0; i < count ; i++)
    {
        QModelIndex oldIndex;

        // Convert the sourceRow (starting with the first or last according to the way we're moving)
        if (destinationChild < sourceRow) oldIndex = index(sourceRow,0);
        else oldIndex = index(sourceEnd,0);

        // Get the sourcemodel index
        oldIndex = mapToSource(oldIndex);

        // Get the target row
        QModelIndex newIndex = index(destinationChild, 0);
        newIndex = mapToSource(newIndex);

        int source = oldIndex.row();
        int dest = newIndex.row();
        int d = dest;
        if (source < dest) d++;

        if (!beginMoveRows(QModelIndex(), source, 1, QModelIndex(), d))
            return false;
        sourceModel()->moveRow(sourceParent, source, destinationParent, dest);
        endMoveRows();
    }
    return true;
}

int DBTableFilterProxyModel::count() const
{
    return rowCount();
}

QString DBTableFilterProxyModel::getUuid(int row) const
{
    QModelIndex i = mapToSource( index(row, 0) );
    QString uuid = m_tableModel->getUuid(i.row());
    if (filterAcceptsUuid(uuid)) return uuid;
    return "";
}

RamObject *DBTableFilterProxyModel::get(int row) const
{
    QModelIndex i = mapToSource( index(row, 0) );
    RamObject *o = m_tableModel->get(i.row());
    if (filterAcceptsObject(o)) return o;
    return nullptr;
}

RamObject *DBTableFilterProxyModel::search(QString searchString) const
{
    RamObject *o = m_tableModel->search(searchString);
    if (filterAcceptsObject(o)) return o;
    else return nullptr;
}

QSet<RamObject *> DBTableFilterProxyModel::lookUp(QString lookUpValue) const
{
    QSet<RamObject *> objs = m_tableModel->lookUp(lookUpValue);
    QMutableSetIterator<RamObject *> it(objs);
    while (it.hasNext())
    {
        it.next();
        RamObject *o = it.value();
        if (!filterAcceptsObject(o)) it.remove();
    }
    return objs;
}

QSet<RamObject *> DBTableFilterProxyModel::lookUpNoFilter(QString lookUpValue) const
{
    return m_tableModel->lookUp(lookUpValue);
}

QVector<QString> DBTableFilterProxyModel::toVector() const
{
    QVector<QString> uuids = m_tableModel->toVector();
    for (int i = uuids.count()-1; i>=0; i--)
    {
        if (!filterAcceptsUuid(uuids.at(i))) uuids.removeAt(i);
    }
    return uuids;
}

QStringList DBTableFilterProxyModel::toStringList() const
{
    QStringList uuids = m_tableModel->toStringList();
    for (int i = uuids.count()-1; i>=0; i--)
    {
        if (!filterAcceptsUuid(uuids.at(i))) uuids.removeAt(i);
    }
    return uuids;
}

bool DBTableFilterProxyModel::contains(QString uuid) const
{
    return filterAcceptsUuid(uuid);
}

RamAbstractObject::ObjectType DBTableFilterProxyModel::type() const
{
    return m_tableModel->type();
}

int DBTableFilterProxyModel::uuidRow(QString uuid) const
{
    if (!filterAcceptsUuid(uuid)) return -1;
    int r = m_tableModel->uuidRow(uuid);
    QModelIndex sourceI = m_tableModel->index(r, 0);
    return mapFromSource(sourceI).row();
}

bool DBTableFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent);

    m_tableModel->load();
    RamObject *o = m_tableModel->get( sourceRow );
    if (!o) return false;
    return checkFilters(o);
}

bool DBTableFilterProxyModel::checkFilters(RamObject *obj) const
{
    if (m_filters.isEmpty()) return true;

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
            if ( !values.contains( obj->getData(key).toString() ) ) return false;
        }

        i++;
    }

    return true;
}

bool DBTableFilterProxyModel::filterAcceptsObject(RamObject *obj) const
{
    if (!obj) return false;
    return checkFilters(obj);
}

bool DBTableFilterProxyModel::filterAcceptsUuid(QString uuid) const
{
    RamObject *o = RamObject::get(uuid, type());
    return filterAcceptsObject(o);
}
