#include "ramscheduleentrymodel.h"

#include "ramscheduleentry.h"
#include "ramstep.h"
#include "ramuser.h"
#include "statemanager.h"

RamScheduleEntryModel::RamScheduleEntryModel(QObject *parent)
    : DBTableModel{RamAbstractObject::ScheduleEntry, true, false, parent}
{
    addLookUpKey("date");
    addLookUpKey("row");
    addLookUpKey("step");

    connect( this, &DBTableModel::rowsInserted, this,&RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::dataChanged, this, &RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::modelReset, this, &RamScheduleEntryModel::countAll);

    connect( StateManager::i(), &StateManager::stateChanged,
            this, [this] (StateManager::State st) {
        freezeEstimation(st != StateManager::Idle);
    });

    countAll();
}

AssignedCount RamScheduleEntryModel::stepCount(const QString &stepUuid)
{
    return m_stepCounts.value(stepUuid);
}

AssignedCount RamScheduleEntryModel::stepUserCount(const QString &userUuid, const QString &stepUuid)
{
    UserAssignedCount uCount = userCount(userUuid);
    return uCount.stepCounts.value(stepUuid);
}

UserAssignedCount RamScheduleEntryModel::userCount(const QString &userUuid)
{
    return m_userCounts.value(userUuid);
}

void RamScheduleEntryModel::freezeEstimation(bool frozen)
{
    m_estimationFrozen = frozen;
    if (!frozen && m_estimationNeedsUpdate) this->countAll();
}

void RamScheduleEntryModel::countAll()
{
    m_estimationNeedsUpdate = true;

    if (m_estimationFrozen) return;

    if (StateManager::i()->isDBBusy()) return;

    m_userCounts.clear();
    m_stepCounts.clear();

    QDate now = QDate::currentDate();

    for (int i = 0; i < this->rowCount(); i++) {
        RamScheduleEntry *entry = RamScheduleEntry::c( this->get(i) );
        if (!entry) continue;
        if (!entry->step()) continue;
        if (!entry->row()->user()) continue;

        QString stepUuid = entry->step()->uuid();
        QString userUuid = entry->row()->user()->uuid();

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
