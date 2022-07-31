#ifndef RAMSCHEDULEENTRY_H
#define RAMSCHEDULEENTRY_H

#include "ramobject.h"
#include "ramuser.h"
#include "ramstep.h"

class RamScheduleEntry : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static RamScheduleEntry *getObject(QString uuid);

    // METHODS

    explicit RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date);

    QString name() const override;
    QString shortName() const override;

    RamUser *user() const;
    RamStep *step() const;
    const QDateTime &date() const;

protected:
    RamScheduleEntry(QString uuid);

private:
    void construct();

    RamUser *m_user = nullptr;
    RamStep *m_step = nullptr;
    QDateTime m_date;
};

#endif // RAMSCHEDULEENTRY_H
