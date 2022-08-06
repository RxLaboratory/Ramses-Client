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

    static RamScheduleEntry *getObject(QString uuid, bool constructNew = false);

    // METHODS

    explicit RamScheduleEntry(RamUser *user, QDateTime date);

    RamUser *user() const;
    const QDateTime &date() const;

    RamStep *step() const;
    void setStep(RamStep *newStep);

protected:
    RamScheduleEntry(QString uuid);

private slots:
    void stepRemoved();

private:
    void construct();
    void connectEvents();

    RamUser *m_user = nullptr;
    QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_H
