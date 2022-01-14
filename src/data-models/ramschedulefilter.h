#ifndef RAMSCHEDULEFILTER_H
#define RAMSCHEDULEFILTER_H

#include <QSortFilterProxyModel>
#include <QtDebug>

#include "ramobjectlist.h"

class RamScheduleFilter : public QSortFilterProxyModel
{
public:
    explicit RamScheduleFilter(QObject *parent = nullptr);
    void setList(QAbstractItemModel *list);

    void ignoreUserUuid(QString uuid);
    void acceptUserUuid(QString uuid);

    void hideDay(int d);
    void showDay(int d);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceCol, const QModelIndex &sourceParent) const override;

private:
    RamObjectList *m_emptyList;

    QStringList m_filterUserUuids;
    QList<int> m_hiddenDays;
};

#endif // RAMSCHEDULEFILTER_H
