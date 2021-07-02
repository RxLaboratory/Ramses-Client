#include "ramobjectfiltermodel.h"

RamObjectFilterModel::RamObjectFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = new RamObjectList(this);
    this->setSourceModel(m_emptyList);
}

void RamObjectFilterModel::setList(QAbstractItemModel *list)
{
    if(!list) this->setSourceModel(m_emptyList);
    this->setSourceModel(list);
}

void RamObjectFilterModel::setFilterUuid(const QString &filterUuid)
{
    m_currentFilterUuid = filterUuid;
    invalidateFilter();
}

void RamObjectFilterModel::search(const QString &searchStr)
{
    m_searchString = searchStr;
    invalidateFilter();
}

bool RamObjectFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RamObject *obj = reinterpret_cast<RamObject*>(iptr);

    bool filterOK = m_currentFilterUuid == "" || obj->filterUuid() == m_currentFilterUuid;
    if (!filterOK) return false;
    if (m_searchString == "") return true;
    if (obj->shortName().contains(m_searchString, Qt::CaseInsensitive)) return true;
    return obj->name().contains(m_searchString, Qt::CaseInsensitive);
}
