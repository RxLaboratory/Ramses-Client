#include "ramscheduleentry.h"

#include "ramproject.h"

// STATIC //

RamScheduleEntry *RamScheduleEntry::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamScheduleEntry(uuid);
    return qobject_cast<RamScheduleEntry*>( obj );
}

// PUBLIC //

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date):
       RamObject(step->shortName(), step->name(), ObjectType::ScheduleEntry, user)
{
    construct();

    m_user = user;
    m_step = step;
    m_date = date;
    m_step->countAssignedDays();

    QJsonObject d = data();
    d.insert("user", user->uuid());
    d.insert("step", step->uuid());
    d.insert("date", date.toString("yyyy-MM-dd hh:mm:ss"));
    setData(d);

    connectEvents();
}

RamUser *RamScheduleEntry::user() const
{
    return m_user;
}

RamStep *RamScheduleEntry::step() const
{
    return m_step;
}

const QDateTime &RamScheduleEntry::date() const
{
    return m_date;
}

// PROTECTED //

RamScheduleEntry::RamScheduleEntry(QString uuid):
    RamObject(uuid, ObjectType::ScheduleEntry)
{
    construct();

    QJsonObject d = data();
    m_user = RamUser::getObject( d.value("user").toString(), true );
    m_step = RamStep::getObject( d.value("step").toString(), true );
    m_date = QDateTime::fromString( d.value("date").toString(), "yyyy-MM-dd hh:mm:ss");

    this->setParent(m_user);

    connectEvents();
}

void RamScheduleEntry::construct()
{
    m_icon = ":/icons/calendar";
    m_editRole = Lead;
}

void RamScheduleEntry::connectEvents()
{
    connect(m_user,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
    connect(m_step,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
    connect(this, SIGNAL(removed(RamObject*)), m_step, SLOT(countAssignedDays()));
}
