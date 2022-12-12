#ifndef RAMSCHEDULEFILTERPROXYMODEL_H
#define RAMSCHEDULEFILTERPROXYMODEL_H

#include "ramscheduletablemodel.h"

class RamScheduleFilterProxyModel : public QSortFilterProxyModel
{
public:
    explicit RamScheduleFilterProxyModel(QObject *parent = nullptr);

    virtual void setSourceSchedule(RamScheduleTableModel *sourceModel);

    void ignoreUserUuid(QString uuid);
    void acceptUserUuid(QString uuid);

    void hideDay(int d);
    void showDay(int d);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceCol, const QModelIndex &sourceParent) const override;

private slots:
    void changeData(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());

private:
    QStringList m_filterUserUuids;
    QList<int> m_hiddenDays;
};

#endif // RAMSCHEDULEFILTERPROXYMODEL_H
