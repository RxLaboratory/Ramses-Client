#include "ramscheduleentrymodel.h"

#include "ramscheduleentry.h"
#include "ramstep.h"
#include "ramuser.h"
#include "statemanager.h"

RamScheduleEntryModel::RamScheduleEntryModel(QObject *parent)
    : DBTableModel{RamAbstractObject::ScheduleEntry, true, false, parent}
{
    setRejectInvalidData(true);
    addLookUpKey("date");
    addLookUpKey("row");
    addLookUpKey("step");

    connect( this, &DBTableModel::rowsInserted, this,&RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::rowsAboutToBeRemoved, this, &RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::dataChanged, this, &RamScheduleEntryModel::countAll);
    connect( this, &DBTableModel::modelReset, this, &RamScheduleEntryModel::countAll);

    connect( StateManager::i(), &StateManager::stateChanged,
            this, [this] (StateManager::State st) {
        suspendEstimations(st != StateManager::Idle);
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

void RamScheduleEntryModel::suspendEstimations(bool frozen)
{
    m_estimationFrozen = frozen;
    if (!frozen && m_estimationNeedsUpdate) this->countAll();
}

void RamScheduleEntryModel::countAll()
{
    m_estimationNeedsUpdate = true;

    if (m_estimationFrozen) return;

    m_userCounts.clear();
    m_stepCounts.clear();

    QDate now = QDate::currentDate();

    // We need to sort the entries per row/date
    QHash <
        RamScheduleRow*,
        QHash <
            QDate,
            QVector<RamScheduleEntry*>
            >
        > sortedEntries;

    for (int i = 0; i < this->rowCount(); i++) {
        RamScheduleEntry *entry = RamScheduleEntry::c( this->get(i) );
        if (!entry) continue;
        if (!entry->step()) continue;
        if (!entry->row()) continue;
        if (!entry->row()->user()) continue;

        auto entriesByDate = sortedEntries.value(entry->row());
        auto entries = entriesByDate.value(entry->date());
        entries << entry;
        entriesByDate.insert(entry->date(), entries);
        sortedEntries.insert(entry->row(), entriesByDate);
    }

    auto it = QHashIterator<
        RamScheduleRow*,
        QHash <
            QDate,
            QVector<RamScheduleEntry*>
            >
        >(sortedEntries);

    while(it.hasNext()) {
        it.next();

        QString userUuid = it.key()->user()->uuid();
        auto entriesByDate = it.value();

        auto itByDate = QHashIterator<
            QDate,
            QVector<RamScheduleEntry*>
            >(entriesByDate);

        while(itByDate.hasNext()) {
            itByDate.next();

            const auto entries = itByDate.value();
            auto date = itByDate.key();

            qreal n = entries.count();
            for(auto entry: entries) {
                QString stepUuid = entry->step()->uuid();

                UserAssignedCount uCount = m_userCounts.value(userUuid);
                AssignedCount userStepCount = uCount.stepCounts.value( stepUuid );
                AssignedCount stepCount = m_stepCounts.value(stepUuid);

                qreal i = 1.0/n;
                uCount.total += i;
                userStepCount.total += i;
                stepCount.total += i;
                if (date >= now) {
                    uCount.future += i;
                    stepCount.future += i;
                    userStepCount.future += i;
                }
                else {
                    uCount.past += i;
                    stepCount.past += i;
                    userStepCount.past += i;
                }

                uCount.stepCounts.insert(stepUuid, userStepCount);
                m_userCounts.insert(userUuid, uCount);
                m_stepCounts.insert(stepUuid, stepCount);
            }
        }
    }

    emit countChanged();

    m_estimationNeedsUpdate = false;
}
