#include "ramscheduleentry.h"

// STATIC //

RamScheduleEntry *RamScheduleEntry::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamScheduleEntry(uuid);
    return qobject_cast<RamScheduleEntry*>( obj );
}

// PUBLIC //

RamScheduleEntry::RamScheduleEntry(RamUser *user, QDateTime date):
       RamObject(user->shortName(), user->name() + " | " + date.toString("yyyy-MM-dd ap"), ObjectType::ScheduleEntry, user)
{
    construct();

    m_user = user;
    m_date = date;

    QJsonObject d = data();
    d.insert("user", user->uuid());
    d.insert("step", "none");
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
    return RamStep::getObject( getData("step").toString(), true);
}

const QDateTime &RamScheduleEntry::date() const
{
    return m_date;
}

void RamScheduleEntry::setStep(RamStep *newStep)
{
    RamStep *currentStep = step();
    if (currentStep)
    {
        disconnect(currentStep, nullptr, this, nullptr);
        disconnect(this, nullptr, currentStep, nullptr);
        currentStep->countAssignedDays();
    }

    if (newStep)
    {
        insertData("step", newStep->uuid());
        connect(this, SIGNAL(removed(RamObject*)), newStep, SLOT(countAssignedDays()));
        connect(newStep, SIGNAL(removed(RamObject*)), this, SLOT(stepRemoved()));
        newStep->countAssignedDays();
    }
    else insertData("step", "none");
}

// PROTECTED //

RamScheduleEntry::RamScheduleEntry(QString uuid):
    RamObject(uuid, ObjectType::ScheduleEntry)
{
    construct();

    QJsonObject d = data();
    m_user = RamUser::getObject( d.value("user").toString(), true );
    m_date = QDateTime::fromString( d.value("date").toString(), "yyyy-MM-dd hh:mm:ss");

    this->setParent(m_user);

    connectEvents();
}

void RamScheduleEntry::stepRemoved()
{
    setStep(nullptr);
}

void RamScheduleEntry::construct()
{
    m_icon = ":/icons/calendar";
    m_editRole = Lead;
}

void RamScheduleEntry::connectEvents()
{
    connect(m_user,SIGNAL(removed(RamObject*)), this, SLOT(remove()));
}
