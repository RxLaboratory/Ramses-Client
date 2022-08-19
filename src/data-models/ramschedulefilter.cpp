#include "ramschedulefilter.h"

#include "ramobjectlist.h"

RamScheduleFilter::RamScheduleFilter(QObject *parent) : QSortFilterProxyModel(parent)
{
    m_emptyList = RamObjectList::emptyList();
    this->setSourceModel(m_emptyList);
}

void RamScheduleFilter::setList(RamScheduleTable *list)
{
    if(!list) this->setSourceModel(m_emptyList);
    this->setSourceModel(list);
}

bool RamScheduleFilter::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    // Check user
    QString userUuid = sourceModel()->headerData(sourceRow, Qt::Vertical, RamObjectList::UUID).toString();
    if(m_filterUserUuids.contains( userUuid )) return false;
    return true;
}

bool RamScheduleFilter::filterAcceptsColumn(int sourceCol, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    // check date
    QDate date = sourceModel()->headerData( sourceCol, Qt::Horizontal, RamObjectList::Date).value<QDate>();

    if (m_hiddenDays.contains(date.dayOfWeek())) return false;
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

void RamScheduleFilter::hideDay(int d)
{
    m_hiddenDays << d;
    invalidateFilter();
}

void RamScheduleFilter::showDay(int d)
{
    m_hiddenDays.removeAll(d);
    invalidateFilter();
}
