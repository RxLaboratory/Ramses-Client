#include "ramschedulemodel.h"

#include "ramscheduleentry.h"

RamScheduleModel::RamScheduleModel(QObject *parent)
    : DBTableModel{RamAbstractObject::ScheduleEntry, true, false, parent}
{
    m_uniqueDataKeys = QStringList({
        "date",
        "user",
        "project"
    });

    connect( this, &DBTableModel::rowsInserted, this,&RamScheduleModel::countAll);
    connect( this, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleModel::countAll);
    connect( this, &DBTableModel::dataChanged, this, &RamScheduleModel::countAll);
    connect( this, &DBTableModel::modelReset, this, &RamScheduleModel::countAll);

    connect( StateManager::i(), &StateManager::stateChanged, this, &RamScheduleModel::stateChanged);

    countAll();
}

AssignedCount RamScheduleModel::stepCount(const QString &stepUuid)
{
    return m_stepCounts.value(stepUuid);
}

AssignedCount RamScheduleModel::stepUserCount(const QString &userUuid, const QString &stepUuid)
{
    UserAssignedCount uCount = userCount(userUuid);
    return uCount.stepCounts.value(stepUuid);
}

UserAssignedCount RamScheduleModel::userCount(const QString &userUuid)
{
    return m_userCounts.value(userUuid);
}

void RamScheduleModel::freezeEstimation(bool frozen)
{
    m_estimationFrozen = frozen;
    if (!frozen) this->countAll();
}

void RamScheduleModel::countAll()
{
    if (m_estimationFrozen) return;

    m_estimationNeedsUpdate = true;
    if (StateManager::i()->isDBBusy()) return;

    m_userCounts.clear();
    m_stepCounts.clear();

    QDateTime now = QDateTime::currentDateTime();

    for (int i = 0; i < this->rowCount(); i++) {
        RamScheduleEntry *entry = RamScheduleEntry::c( this->get(i) );
        if (!entry) continue;
        if (!entry->step()) continue;
        if (!entry->user()) continue;

        QString stepUuid = entry->step()->uuid();
        QString userUuid = entry->user()->uuid();

        UserAssignedCount uCount = m_userCounts.value(userUuid);
        AssignedCount userStepCount = uCount.stepCounts.value( stepUuid );
        AssignedCount stepCount = m_stepCounts.value(stepUuid);

        uCount.total += 0.5;
        userStepCount.total += 0.5;
        stepCount.total += 0.5;
        if (entry->date() >= now) {
            uCount.future += 0.5;
            stepCount.future += 0.5;
            userStepCount.future += 0.5;
        }
        else {
            uCount.past += 0.5;
            stepCount.past += 0.5;
            userStepCount.past += 0.5;
        }

        uCount.stepCounts.insert(stepUuid, userStepCount);
        m_userCounts.insert(userUuid, uCount);
        m_stepCounts.insert(stepUuid, stepCount);
    }

    emit countChanged();

    m_estimationNeedsUpdate = false;
}

void RamScheduleModel::stateChanged(StateManager::State state)
{
    if (state == StateManager::Idle && m_estimationNeedsUpdate) countAll();
}
