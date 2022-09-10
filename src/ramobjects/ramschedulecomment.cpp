#include "ramschedulecomment.h"

// STATIC //

QMap<QString, RamScheduleComment*> RamScheduleComment::m_existingObjects = QMap<QString, RamScheduleComment*>();

RamScheduleComment *RamScheduleComment::get(QString uuid)
{
    if (!checkUuid(uuid, ScheduleComment)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamScheduleComment(uuid);
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

QVariant RamScheduleComment::roleData(int role) const
{
    switch (role)
    {
    case Qt::DisplayRole: return this->comment();
    case Qt::ToolTipRole: {
        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", "yyyy-MM-dd").toString();
        return this->date().toString(dateFormat) + "\n" + this->comment();
    }
    case Qt::StatusTipRole: {
        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", "yyyy-MM-dd").toString();
        return this->date().toString(dateFormat) + "\n" + this->comment();
    }
    case Qt::BackgroundRole: return QBrush(this->color());
    case IsPM: return false;
    case Date: return this->date();
    case IsComment: return true;
    case SizeHint: return QSize(75,10);
    }
    return RamObject::roleData(role);
}

// PRIVATE //

void RamScheduleComment::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/calendar";
    m_editRole = Standard;
}









