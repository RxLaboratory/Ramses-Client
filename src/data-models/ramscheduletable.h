#ifndef RAMSCHEDULETABLE_H
#define RAMSCHEDULETABLE_H

#include <QStringBuilder>

#include "ramobjectlist.h"

class RamScheduleTable : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RamScheduleTable(QObject *parent = nullptr);
    void setList(RamObjectList *userList, RamObjectList *comments);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;



public slots:
    void setStartDate(const QDate &newStartDate);
    void setEndDate(const QDate &newEndDate);

private slots:
    // TODO Connect user schedule datachanged, inserted, removed, to data changed (disconnect when remove user)
    void insertUser(const QModelIndex &parent, int first, int last);
    void removeUser(const QModelIndex &parent, int first, int last);
    void resetUsers();

private:
     RamObjectList *m_users = nullptr;
     RamObjectList *m_comments = nullptr;

     QDate m_startDate;
     QDate m_endDate;

     // Connect submodels and relay events
     void connectEvents();
};

#endif // RAMSCHEDULETABLE_H
