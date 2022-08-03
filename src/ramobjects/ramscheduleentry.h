#ifndef RAMSCHEDULEENTRY_H
#define RAMSCHEDULEENTRY_H

#include "ramobject.h"

class RamUser;
class RamStep;

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleEntry *getObject(QString uuid, bool constructNew = false);

    // METHODS

    explicit RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date);

    RamUser *user() const;
    RamStep *step() const;
    const QDateTime &date() const;

protected:
    RamScheduleEntry(QString uuid);

private:
    void construct();
    void connectEvents();

    RamUser *m_user = nullptr;
    RamStep *m_step = nullptr;
    QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_H
