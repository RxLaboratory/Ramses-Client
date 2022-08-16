#ifndef RAMSCHEDULEENTRY_H
#define RAMSCHEDULEENTRY_H

#include "ramobject.h"

#include "ramstep.h"
#include "ramuser.h"

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleEntry *get(QString uuid);
    static RamScheduleEntry* c(RamObject *o);

    // METHODS

    explicit RamScheduleEntry(RamUser *user, QDateTime date);
    RamScheduleEntry(QString uuid);

    RamUser *user() const;
    const QDateTime &date() const;

    RamStep *step() const;
    void setStep(RamStep *newStep);

private slots:
    void stepRemoved();

private:
    void construct();
    void connectEvents();

    RamUser *m_user = nullptr;
    QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_H
