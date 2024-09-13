#ifndef RAMSCHEDULEENTRYMODEL_H
#define RAMSCHEDULEENTRYMODEL_H

#include "dbtablemodel.h"

struct AssignedCount {
    qreal total = 0;
    qreal future = 0;
    qreal past = 0;
};

struct UserAssignedCount {
    QHash<QString, AssignedCount> stepCounts;
    qreal total = 0;
    qreal future = 0;
    qreal past = 0;
};

/**
 * @brief The RamScheduleEntryModel class is the list of all schedule entries for a project
 */
class RamScheduleEntryModel : public DBTableModel
{
    Q_OBJECT
public:
    explicit RamScheduleEntryModel(QObject *parent = nullptr);

    // COUNTS
    AssignedCount stepCount(const QString &stepUuid);
    AssignedCount stepUserCount(const QString &userUuid, const QString &stepUuid);
    UserAssignedCount userCount(const QString &userUuid);

signals:
    void countChanged();

public slots:
    void suspendEstimations(bool frozen = true);

private slots:
    // Counts
    void countAll();

private:
    // COUNTS
    QHash<QString, UserAssignedCount> m_userCounts;
    QHash<QString, AssignedCount> m_stepCounts;

    bool m_estimationFrozen = false;
    bool m_estimationNeedsUpdate = false;
};

#endif // RAMSCHEDULEENTRYMODEL_H
