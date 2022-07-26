#include "ramscheduleentry.h"

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date):
       RamObject(user)
{
    m_icon = ":/icons/calendar";
    m_editRole = Lead;

    this->setObjectType(ScheduleEntry);
    setUser(user);
    setStep(step);
    m_date = date;
    m_step->countAssignedDays();
    m_dbi->createSchedule( user->uuid(), step->uuid(), date, m_uuid );
    this->setObjectName("RamScheduleEntry " + user->shortName() + " | " + step->shortName());
}

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date, QString uuid) :
    RamObject(uuid, user)
{
    this->setObjectType(ScheduleEntry);
    setUser(user);
    setStep(step);
    m_date = date;
    m_step->countAssignedDays();
    m_dbi->createSchedule( user->uuid(), step->uuid(), date, m_uuid );
    this->setObjectName("RamScheduleEntry " + user->shortName() + " | " + step->shortName());
}

RamScheduleEntry::~RamScheduleEntry()
{
    //if (m_step) m_step->countAssignedDays();
}

QString RamScheduleEntry::name() const
{
    return m_step->name();
}

QString RamScheduleEntry::shortName() const
{
    return m_step->shortName();
}

RamUser *RamScheduleEntry::user() const
{
    return m_user;
}

void RamScheduleEntry::setUser(RamUser *newUser)
{
    if (newUser->is(m_user)) return;

    m_dirty = true;

    m_user = newUser;

    connect(m_user,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
    emit dataChanged(this);
}

RamStep *RamScheduleEntry::step() const
{
    return m_step;
}

void RamScheduleEntry::setStep(RamStep *newStep)
{
    if (newStep->is(m_step)) return;
    // update assigned days for the previous step
    if (m_step) m_step->countAssignedDays();

    // Disconnect
    disconnect(m_stepConnection);

    m_dirty = true;

    m_step = newStep;
    // update assigned days for the new step (if not nullptr)
    if (m_step)
    {
        m_step->countAssignedDays();

        connect(m_step,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
        m_stepConnection = connect(this, SIGNAL(removed(RamObject*)), m_step, SLOT(countAssignedDays()));
    }

    emit dataChanged(this);
}

const QDateTime &RamScheduleEntry::date() const
{
    return m_date;
}

void RamScheduleEntry::setDate(const QDateTime &newDate)
{
    if (newDate == m_date) return;
    m_dirty = true;
    m_date = newDate;
    emit dataChanged(this);
}

ScheduleEntryStruct RamScheduleEntry::toStruct() const
{
    ScheduleEntryStruct s;
    s.uuid = m_uuid;
    s.userUuid = m_user->uuid();
    s.stepUuid = m_step->uuid();
    s.comment = m_comment;
    s.date = m_date;
    return s;
}

RamScheduleEntry *RamScheduleEntry::scheduleEntry(QString uuid)
{
    RamObject *obj = RamObject::obj(uuid);
    return qobject_cast<RamScheduleEntry*>( obj );
}

void RamScheduleEntry::update()
{
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateSchedule(m_uuid, m_user->uuid(), m_step->uuid(), m_date, m_comment );
}

void RamScheduleEntry::removeFromDB()
{
    m_dbi->removeSchedule(m_uuid);
}
