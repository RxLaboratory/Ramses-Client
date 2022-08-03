#include "ramobjectfiltermodel.h"

RamObjectFilterModel::RamObjectFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = RamObjectList<RamObject *>::getObject("emptylist", true);
    setSourceModel(m_emptyList);
}

void RamObjectFilterModel::setList(QAbstractItemModel *list)
{
    if (this->sourceModel() == list) return;

    if (!list) this->setSourceModel(m_emptyList);
    else this->setSourceModel(list);

    // Refresh
    QString f = m_currentFilterUuid;
    setFilterUuid("");
    setFilterUuid(f);
}

void RamObjectFilterModel::setFilterUuid(const QString &filterUuid)
{
    m_currentFilterUuid = filterUuid;
    prepareFilter();
}

void RamObjectFilterModel::search(const QString &searchStr)
{
    m_searchString = searchStr;
    prepareFilter();
}

bool RamObjectFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);
    if (!obj) return false;

    bool filterOK = m_currentFilterUuid == "" || obj->filterUuid() == m_currentFilterUuid;
    if (!filterOK) return false;
    if (m_searchString == "") return true;
    if (obj->shortName().contains(m_searchString, Qt::CaseInsensitive)) return true;
    return obj->name().contains(m_searchString, Qt::CaseInsensitive);
}

void RamObjectFilterModel::prepareFilter()
{
    emit aboutToFilter();
    invalidateFilter();
}
