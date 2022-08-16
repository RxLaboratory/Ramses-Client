#include "ramschedulecomment.h"

// STATIC //

RamScheduleComment *RamScheduleComment::get(QString uuid)
{
    return c( RamObject::get(uuid, ScheduleComment) );
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

RamScheduleComment::RamScheduleComment(QString uuid):
    RamObject(uuid, ObjectType::ScheduleComment)
{
    construct();

    QJsonObject d = data();
    m_project = RamProject::get( d.value("project").toString() );

    this->setParent(m_project);
}

QDateTime RamScheduleComment::date() const
{
    return QDateTime::fromString( getData("date").toString(), "yyyy-MM-dd hh:mm:ss" );
}

void RamScheduleComment::setDate(const QDateTime &newDate)
{
    insertData("date", newDate.toString("yyyy-MM-dd hh:mm:ss"));
}

// PRIVATE //

void RamScheduleComment::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = Standard;
}









