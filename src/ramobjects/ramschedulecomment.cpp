#include "ramschedulecomment.h"

#include "ramproject.h"

// STATIC //


RamScheduleComment *RamScheduleComment::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamScheduleComment( uuid );
    return qobject_cast<RamScheduleComment*>( obj );
}

// PUBLIC //

RamScheduleComment::RamScheduleComment(RamProject *project)
    : RamObject("schdlCmnt", "Schedule Comment", ObjectType::ScheduleComment, project)
{
    m_project = project;

    insertData("project", project->uuid() );
}

QColor RamScheduleComment::color() const
{
    return QColor( data().value("color").toString("#e3e3e3") );
}

void RamScheduleComment::setColor(const QColor &newColor)
{
    insertData( "color", newColor.name() );
}

QDateTime RamScheduleComment::date() const
{
    return QDateTime::fromString( data().value("date").toString(), "yyyy-MM-dd hh:mm:ss" );
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









