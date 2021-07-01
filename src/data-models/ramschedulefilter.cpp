#include "ramschedulefilter.h"

RamScheduleFilter::RamScheduleFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = new RamObjectList();
    this->setSourceModel(m_emptyList);
}

void RamScheduleFilter::setList(QAbstractItemModel *list)
{
    if(!list) this->setSourceModel(m_emptyList);
    this->setSourceModel(list);
}

bool RamScheduleFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    // Check user
    QString userUuid = sourceModel()->headerData(sourceRow, Qt::Vertical, Qt::UserRole +2).toString();
    if(m_filterUserUuids.contains( userUuid )) return false;
    return true;
}

void RamScheduleFilter::ignoreUserUuid(QString uuid)
{
    if (!m_filterUserUuids.contains(uuid))
    {
        m_filterUserUuids << uuid;
        invalidateFilter();
    }
}

void RamScheduleFilter::acceptUserUuid(QString uuid)
{
    m_filterUserUuids.removeAll(uuid);
    invalidateFilter();
}
