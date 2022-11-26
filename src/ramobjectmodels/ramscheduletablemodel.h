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
    void setObjectModel(RamObjectModel *userList, DBTableModel *comments);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void setStartDate(const QDate &newStartDate);
    void setEndDate(const QDate &newEndDate);

private slots:
    void insertUser(const QModelIndex &parent, int first, int last);
    void removeUser(const QModelIndex &parent, int first, int last);
    void resetUsers();

    void scheduleChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles = QVector<int>());
    void scheduleInserted(const QModelIndex &parent, int first, int last);
    void scheduleRemoved(const QModelIndex &parent, int first, int last);

    void insertComment(const QModelIndex &parent, int first, int last);
    void removeComment(const QModelIndex &parent, int first, int last);
    void resetComments();

private:
    QModelIndex entryIndex(RamScheduleEntry *e);
    int colForDate(QDateTime date);

    // DATA
    RamObjectModel *m_users = nullptr;
    DBTableModel *m_comments = nullptr;

    // SETTINGS
    QDate m_startDate;
    QDate m_endDate;
};

#endif // RAMSCHEDULETABLEMODEL_H
