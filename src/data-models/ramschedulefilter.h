#ifndef RAMSCHEDULEFILTER_H
#define RAMSCHEDULEFILTER_H

#include <QSortFilterProxyModel>
#include <QtDebug>

template<typename RO> class RamObjectList;
class RamScheduleEntry;

class RamScheduleFilter : public QSortFilterProxyModel
{
public:
    explicit RamScheduleFilter(QObject *parent = nullptr);
    void setList(RamObjectList<RamScheduleEntry *> *list);

    void ignoreUserUuid(QString uuid);
    void acceptUserUuid(QString uuid);

    void hideDay(int d);
    void showDay(int d);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;
    bool filterAcceptsColumn(int sourceCol, const QModelIndex &sourceParent) const override;

private:
    RamObjectList<RamScheduleEntry*> *m_emptyList;

    QStringList m_filterUserUuids;
    QList<int> m_hiddenDays;
};

#endif // RAMSCHEDULEFILTER_H
