#ifndef RAMSCHEDULETABLEMODEL_H
#define RAMSCHEDULETABLEMODEL_H

#include <QStringBuilder>
#include "dbtablemodel.h"
#include "ramobjectmodel.h"

class RamUser;
class RamScheduleEntry;

class RamScheduleTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit RamScheduleTableModel(QObject *parent = nullptr);
    void setObjectModel(RamObjectModel *users, DBTableModel *schedule, DBTableModel *comments);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void setStartDate(const QDate &newStartDate);
    void setEndDate(const QDate &newEndDate);

private slots:
    void resetUsers();
    void changeUser(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void insertUser(const QModelIndex &parent, int first, int last);
    void removeUser(const QModelIndex &parent, int first, int last);

    void resetSchedule();
    void changeSchedule(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void insertSchedule(const QModelIndex &parent, int first, int last);
    void removeSchedule(const QModelIndex &parent, int first, int last);

    void changeComment(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void insertComment(const QModelIndex &parent, int first, int last);
    void removeComment(const QModelIndex &parent, int first, int last);
    void resetComments();

private:
    QModelIndex entryIndex(RamScheduleEntry *e);
    int colForDate(QDateTime date);

    // DATA
    RamObjectModel *m_users = nullptr;
    DBTableModel *m_schedule = nullptr;
    DBTableModel *m_comments = nullptr;

    // SETTINGS
    QDate m_startDate;
    QDate m_endDate;
};

#endif // RAMSCHEDULETABLEMODEL_H
