#include "ramschedulefilterproxymodel.h"

#include "ramobject.h"

RamScheduleFilterProxyModel::RamScheduleFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

void RamScheduleFilterProxyModel::setSourceSchedule(RamScheduleTableModel *sourceModel)
{
    QAbstractItemModel *current = this->sourceModel();
    if (current) disconnect(current, nullptr, this, nullptr);
    // For some reason, data changed is not forwarded? Do it ourselves
    connect(sourceModel, &RamScheduleTableModel::dataChanged, this, &RamScheduleFilterProxyModel::changeData);

    QSortFilterProxyModel::setSourceModel(sourceModel);
}

bool RamScheduleFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    // Check user
    QString userUuid = sourceModel()->headerData(sourceRow, Qt::Vertical, RamObject::UUID).toString();
    if(m_filterUserUuids.contains( userUuid )) return false;
    return true;
}

bool RamScheduleFilterProxyModel::filterAcceptsColumn(int sourceCol, const QModelIndex &sourceParent) const
{
    Q_UNUSED(sourceParent)

    // check date
    QDate date = sourceModel()->headerData( sourceCol, Qt::Horizontal, RamObject::Date).value<QDate>();

    if (m_hiddenDays.contains(date.dayOfWeek())) return false;
    return true;
}

void RamScheduleFilterProxyModel::changeData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
    QModelIndex tl = mapFromSource(topLeft);
    QModelIndex br = mapFromSource(bottomRight);
    emit dataChanged(tl, br, roles);
}

void RamScheduleFilterProxyModel::ignoreUserUuid(QString uuid)
{
    if (!m_filterUserUuids.contains(uuid))
    {
        m_filterUserUuids << uuid;
        invalidateFilter();
    }
}

void RamScheduleFilterProxyModel::acceptUserUuid(QString uuid)
{
    m_filterUserUuids.removeAll(uuid);
    invalidateFilter();
}

void RamScheduleFilterProxyModel::hideDay(int d)
{
    m_hiddenDays << d;
    invalidateFilter();
}

void RamScheduleFilterProxyModel::showDay(int d)
{
    m_hiddenDays.removeAll(d);
    invalidateFilter();
}
