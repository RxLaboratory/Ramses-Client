#include "ramscheduleentry.h"

#include "scheduleentryeditwidget.h"

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date):
       RamObject(user)
{
    this->setObjectType(ScheduleEntry);
    m_user = user;
    m_step = step;
    m_date = date;
    m_dbi->createSchedule( user->uuid(), step->uuid(), date, m_uuid );
    this->setObjectName("RamScheduleEntry " + user->shortName() + " | " + step->shortName());
}

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date, QString uuid) :
    RamObject(uuid, user)
{
    this->setObjectType(ScheduleEntry);
    m_user = user;
    m_step = step;
    m_date = date;
    m_dbi->createSchedule( user->uuid(), step->uuid(), date, m_uuid );
    this->setObjectName("RamScheduleEntry " + user->shortName() + " | " + step->shortName());
}

RamScheduleEntry::~RamScheduleEntry()
{

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
    emit changed(this);
}

RamStep *RamScheduleEntry::step() const
{
    return m_step;
}

void RamScheduleEntry::setStep(RamStep *newStep)
{
    if (newStep->is(m_step)) return;
    m_dirty = true;
    m_step = newStep;

    connect(m_step,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
    emit changed(this);
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
    emit changed(this);
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
    return qobject_cast<RamScheduleEntry*>( RamObject::obj(uuid) );
}

void RamScheduleEntry::update()
{
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateSchedule(m_uuid, m_user->uuid(), m_step->uuid(), m_date, m_comment );
}

void RamScheduleEntry::edit(bool show)
{
    if (!m_editReady)
    {
        ScheduleEntryEditWidget *w = new ScheduleEntryEditWidget(this);
        setEditWidget(w);
        m_editReady = true;//*/
    }
    showEdit(show);
}

void RamScheduleEntry::removeFromDB()
{
    m_dbi->removeSchedule(m_uuid);
}
