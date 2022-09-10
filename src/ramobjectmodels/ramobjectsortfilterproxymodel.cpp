#include "ramobjectsortfilterproxymodel.h"

#include "ramobjectmodel.h"

RamObjectSortFilterProxyModel::RamObjectSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{

}

RamObjectSortFilterProxyModel::RamObjectSortFilterProxyModel(QString listName, QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_listName = listName;
}

void RamObjectSortFilterProxyModel::setSingleColumn(bool singleColumn)
{
    m_isSingleColumn = singleColumn;
}

int RamObjectSortFilterProxyModel::columnCount(const QModelIndex &parent) const
{
    if (m_isSingleColumn) return 1;
    return QSortFilterProxyModel::columnCount(parent);
}

QVariant RamObjectSortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    if (columnCount() == 0) return QVariant();

#if QT_VERSION > QT_VERSION_CHECK(5, 11, 0)
   if (!checkIndex(index, CheckIndexOption::IndexIsValid))
            return QVariant();
#endif

    return QSortFilterProxyModel::data( index, role);
}

RamObject *RamObjectSortFilterProxyModel::get(int row) const
{
    return get(index(row, 0));
}

RamObject *RamObjectSortFilterProxyModel::get(QModelIndex index) const
{
    QString uuid = index.data(Qt::UserRole).toString();
    if (uuid == "") return nullptr;

    RamObject::ObjectType t = type();
    return RamObject::get(uuid, t);
}

RamObject *RamObjectSortFilterProxyModel::searchObject(QString searchString) const
{
    // Shortname first
    for (int i = 0; i < rowCount(); i++)
    {
        RamObject *o = get(i);
        if (!o) continue;
        if (o->shortName() == searchString) return o;
    }
    // Name after
    for (int i = 0; i < rowCount(); i++)
    {
        RamObject *o = get(i);
        if (!o) continue;
        if (o->name() == searchString) return o;
    }
    return nullptr;
}

RamAbstractObject::ObjectType RamObjectSortFilterProxyModel::type() const
{
    RamObjectModel *model = qobject_cast<RamObjectModel*>( sourceModel() );
    if (!model) return RamObject::Object;

    return model->type();
}

void RamObjectSortFilterProxyModel::setFilterUuid(const QString &filterUuid)
{
    m_currentFilterUuid = filterUuid;
    prepareFilter();
}

void RamObjectSortFilterProxyModel::search(const QString &searchStr)
{
    m_searchString = searchStr;
    prepareFilter();
}

void RamObjectSortFilterProxyModel::addFilterUuid(const QString &uuid)
{
    if(!m_filterListUuids.contains(uuid))
    {
        m_filterListUuids << uuid;
    }
    prepareFilter();
}

void RamObjectSortFilterProxyModel::removeFilterUuid(const QString &uuid)
{
    m_filterListUuids.removeAll(uuid);
    prepareFilter();
}

void RamObjectSortFilterProxyModel::clearFilterListUuids()
{
    m_filterListUuids.clear();
    prepareFilter();
}

bool RamObjectSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    RamObjectModel *model = qobject_cast<RamObjectModel*>(sourceModel());
    RamObject *obj = model->get(sourceRow);
    if (!obj) return false;

    // filter uuid
    bool filterOK = m_currentFilterUuid == "" || obj->filterUuid() == m_currentFilterUuid;
    if (!filterOK) return false;

    // search
    if (m_searchString == "") filterOK = true;
    else if (obj->shortName().contains(m_searchString, Qt::CaseInsensitive)) filterOK =  true;
    else filterOK = obj->name().contains(m_searchString, Qt::CaseInsensitive);
    if (!filterOK) return false;

    // filter list uuids
    QStringList uuids = obj->filterListUuids();
    if (m_filterListUuids.count() == 0) filterOK = true;
    else {
        for ( int i = 0; i < m_filterListUuids.count(); i++)
        {
            if (uuids.contains(m_filterListUuids.at(i))) filterOK = true;
        }
    }
    if (!filterOK) return false;

    // All tests passed
    return true;
}

void RamObjectSortFilterProxyModel::prepareFilter()
{
    emit aboutToFilter();
    invalidateFilter();
}
