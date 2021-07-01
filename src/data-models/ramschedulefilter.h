#ifndef RAMSCHEDULEFILTER_H
#define RAMSCHEDULEFILTER_H

#include <QSortFilterProxyModel>
#include <QtDebug>

#include "ramobjectlist.h"

class RamScheduleFilter : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit RamScheduleFilter(QObject *parent = nullptr);
    void setList(QAbstractItemModel *list);

    void ignoreUserUuid(QString uuid);
    void acceptUserUuid(QString uuid);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override;

private:
    RamObjectList *m_emptyList;

    QStringList m_filterUserUuids;
};

#endif // RAMSCHEDULEFILTER_H
