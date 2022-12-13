#ifndef RAMSCHEDULEMODEL_H
#define RAMSCHEDULEMODEL_H

#include "dbtablemodel.h"
#include "statemanager.h"

struct AssignedCount {
    float total = 0;
    float future = 0;
    float past = 0;
};

struct UserAssignedCount {
    QHash<QString, AssignedCount> stepCounts;
    float total = 0;
    float future = 0;
    float past = 0;
};

class RamScheduleModel : public DBTableModel
{
    Q_OBJECT
public:
    explicit RamScheduleModel(QObject *parent = nullptr);

    // COUNTS
    AssignedCount stepCount(QString stepUuid);
    AssignedCount stepUserCount(QString userUuid, QString stepUuid);
    UserAssignedCount userCount(QString userUuid);

signals:
    void countChanged();

public slots:
    void freezeEstimation(bool frozen = true);

private slots:
    // Counts
    void countAll();
    void stateChanged(StateManager::State state);

private:
    // COUNTS
    QHash<QString, UserAssignedCount> m_userCounts;
    QHash<QString, AssignedCount> m_stepCounts;

    bool m_estimationFrozen = false;
    bool m_estimationNeedsUpdate = false;
};

#endif // RAMSCHEDULEMODEL_H
