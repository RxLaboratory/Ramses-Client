#ifndef RAMSCHEDULETABLEMODEL_H
#define RAMSCHEDULETABLEMODEL_H

#include <QStringBuilder>
#include "ramobjectmodel.h"

class RamScheduleTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit RamScheduleTableModel(QObject *parent = nullptr);
    void setObjectModel(RamObjectModel *userList, RamObjectModel *comments);

    // MODEL REIMPLEMENTATION
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public slots:
    void setStartDate(const QDate &newStartDate);
    void setEndDate(const QDate &newEndDate);

private slots:
    void userDataChanged(RamObject *obj);
    void insertUser(const QModelIndex &parent, int first, int last);
    void removeUser(const QModelIndex &parent, int first, int last);
    void disconnectUsers();
    void resetUsers();

    void commentDataChanged(RamObject *obj);
    void insertComment(const QModelIndex &parent, int first, int last);
    void removeComment(const QModelIndex &parent, int first, int last);
    void disconnectComments();
    void resetComments();

private:
     RamObjectModel *m_users = nullptr;
     RamObjectModel *m_comments = nullptr;

     QDate m_startDate;
     QDate m_endDate;

     // Connect submodels and relay events
     void connectEvents();

     void connectUser(int i);
     void disconnectUser(int i);

     void connectComment(int i);
     void disconnectComment(int i);
};

#endif // RAMSCHEDULETABLEMODEL_H
