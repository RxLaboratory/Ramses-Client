#include "ramobjectfiltermodel.h"

template<typename RO>
RamObjectFilterModel<RO>::RamObjectFilterModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = RamObjectList<RO>::getObject("emptylist", true);
    setSourceModel(m_emptyList);
}

template<typename RO>
void RamObjectFilterModel<RO>::setList(RamObjectList<RO> *list)
{
    if (this->sourceModel() == list) return;

    if (!list) this->setSourceModel(m_emptyList);
    else this->setSourceModel(list);

    // Refresh
    QString f = m_currentFilterUuid;
    setFilterUuid("");
    setFilterUuid(f);
}

template<typename RO>
void RamObjectFilterModel<RO>::setList(RamItemTable *list)
{
    if (!list) this->setSourceModel(m_emptyList);
    else this->setSourceModel(list);

    // Refresh
    QString f = m_currentFilterUuid;
    setFilterUuid("");
    setFilterUuid(f);
}

template<typename RO>
void RamObjectFilterModel<RO>::setFilterUuid(const QString &filterUuid)
{
    m_currentFilterUuid = filterUuid;
    prepareFilter();
}

template<typename RO>
void RamObjectFilterModel<RO>::search(const QString &searchStr)
{
    m_searchString = searchStr;
    prepareFilter();
}

template<typename RO>
RO RamObjectFilterModel<RO>::at(int i) const
{
    // Get the source index
    if (!hasIndex(i, 0)) return nullptr;

    QModelIndex filterIndex = index(i, 0);
    QModelIndex sourceIndex = mapToSource(filterIndex);
    RamObjectList<RO> *list = qobject_cast<RamObjectList<RO> *>(this->sourceModel());
    return list->at(sourceIndex.row());
}

template<typename RO>
bool RamObjectFilterModel<RO>::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    quintptr iptr = index.data(Qt::UserRole).toULongLong();
    if (iptr == 0) return false;
    RO obj = reinterpret_cast<RO>(iptr);
    if (!obj) return false;

    bool filterOK = m_currentFilterUuid == "" || obj->filterUuid() == m_currentFilterUuid;
    if (!filterOK) return false;
    if (m_searchString == "") return true;
    if (obj->shortName().contains(m_searchString, Qt::CaseInsensitive)) return true;
    return obj->name().contains(m_searchString, Qt::CaseInsensitive);
}

template<typename RO>
void RamObjectFilterModel<RO>::prepareFilter()
{
    emit aboutToFilter();
    invalidateFilter();
}
