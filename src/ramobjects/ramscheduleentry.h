#ifndef RAMSCHEDULEENTRY_H
#define RAMSCHEDULEENTRY_H

#include "ramobject.h"
#include "ramuser.h"
#include "ramstep.h"

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:
    explicit RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date);
    explicit RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date, QString uuid);
    ~RamScheduleEntry();

    QString name() const override;
    QString shortName() const override;

    RamUser *user() const;
    void setUser(RamUser *newUser);

    RamStep *step() const;
    void setStep(RamStep *newStep);

    const QDateTime &date() const;
    void setDate(const QDateTime &newDate);

    struct ScheduleEntryStruct toStruct() const;

    static RamScheduleEntry *scheduleEntry(QString uuid);

public slots:
    void update() override;
    virtual void removeFromDB() override;

private:
    RamUser *m_user = nullptr;
    RamStep *m_step = nullptr;
    QMetaObject::Connection m_stepConnection;
    QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_H
