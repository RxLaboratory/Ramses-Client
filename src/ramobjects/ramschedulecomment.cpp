#include "ramschedulecomment.h"

RamScheduleComment::RamScheduleComment(RamProject *project, QDateTime date)
    : RamObject(project)
{
    m_date = date;
    m_comment = "";
    m_color = QColor(227, 227, 227);
    m_project = project;

    m_dbi->updateScheduleComment(m_uuid, project->uuid(), m_date, m_comment, m_color);
    this->setObjectType(ScheduleComment);
    this->setObjectName("RamScheduleComment " + project->shortName() );
}

RamScheduleComment::RamScheduleComment(RamProject *project, QDateTime date, QString uuid)
    : RamObject(uuid, project)
{
    m_date = date;
    m_comment = "";
    m_color = QColor(227, 227, 227);
    m_project = project;

    m_dbi->updateScheduleComment(m_uuid, project->uuid(), m_date, m_comment, m_color);
    this->setObjectType(ScheduleComment);
    this->setObjectName("RamScheduleComment " + project->shortName() );
}

const QString &RamScheduleComment::comment() const
{
    return m_comment;
}

void RamScheduleComment::setComment(const QString &newComment)
{
    if (newComment == m_comment) return;
    m_dirty = true;
    m_comment = newComment;
    emit changed(this);
}

const QColor &RamScheduleComment::color() const
{
    return m_color;
}

void RamScheduleComment::setColor(const QColor &newColor)
{
    if (m_color == newColor) return;
    m_dirty = true;
    m_color = newColor;
    emit changed(this);
}

const QDateTime &RamScheduleComment::date() const
{
    return m_date;
}

void RamScheduleComment::setDate(const QDateTime &newDate)
{
    if (m_date == newDate) return;
    m_dirty = true;
    m_date = newDate;
    emit changed(this);
}

ScheduleCommentStruct RamScheduleComment::toStruct() const
{
    ScheduleCommentStruct s;
    s.uuid = m_uuid;
    s.projectUuid = m_project->uuid();
    s.comment = m_comment;
    s.color = m_color;
    s.date = m_date;
    return s;
}

RamScheduleComment *RamScheduleComment::scheduleComment(QString uuid)
{
    RamObject *obj = RamObject::obj(uuid);
    return qobject_cast<RamScheduleComment*>( obj );
}

void RamScheduleComment::update()
{
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateScheduleComment(m_uuid, m_project->uuid(), m_date, m_comment, m_color);
}

void RamScheduleComment::removeFromDB()
{
    m_dbi->updateScheduleComment(m_uuid, m_project->uuid(), m_date, "", QColor(227, 227, 227));
}
