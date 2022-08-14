#include "ramschedulecomment.h"

// STATIC //

RamScheduleComment *RamScheduleComment::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamScheduleComment( uuid );
    return qobject_cast<RamScheduleComment*>( obj );
}

RamScheduleComment *RamScheduleComment::c(RamObject *o)
{
    return qobject_cast<RamScheduleComment*>(o);
}

// PUBLIC //

RamScheduleComment::RamScheduleComment(RamProject *project)
    : RamObject("schdlCmnt", "Schedule Comment", ObjectType::ScheduleComment, project)
{
    m_project = project;

    insertData("project", project->uuid() );
}

QDateTime RamScheduleComment::date() const
{
    return QDateTime::fromString( getData("date").toString(), "yyyy-MM-dd hh:mm:ss" );
}

void RamScheduleComment::setDate(const QDateTime &newDate)
{
    insertData("date", newDate.toString("yyyy-MM-dd hh:mm:ss"));
}

// PROTECTED //

RamScheduleComment::RamScheduleComment(QString uuid):
    RamObject(uuid, ObjectType::ScheduleComment)
{
    construct();

    QJsonObject d = data();
    m_project = RamProject::getObject( d.value("project").toString(), true);

    this->setParent(m_project);
}

// PRIVATE //

void RamScheduleComment::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = Standard;
}









