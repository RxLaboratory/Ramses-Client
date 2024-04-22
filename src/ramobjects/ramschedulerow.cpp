#include "ramschedulerow.h"
#include "ramuser.h"
#include "scheduleroweditwidget.h"

QFrame *RamScheduleRow::ui_editWidget = nullptr;

QHash<QString, RamScheduleRow*> RamScheduleRow::m_existingObjects = QHash<QString, RamScheduleRow*>();

RamScheduleRow *RamScheduleRow::get(QString uuid)
{
    if (!checkUuid(uuid, ScheduleRow)) return nullptr;

    RamScheduleRow *r = m_existingObjects.value(uuid);
    if (r) return r;

    // Finally return a new instance
    return new RamScheduleRow(uuid);
}

RamScheduleRow *RamScheduleRow::c(RamObject *o, bool fast)
{
    // For performance, reinterpret_cast, but be careful with the object passed!
    if (fast)
        return reinterpret_cast<RamScheduleRow*>(o);
    // Otherwise use the safer qobject_cast
    return qobject_cast<RamScheduleRow*>(o);
}

RamScheduleRow::RamScheduleRow(const QString &shortName, const QString &name, RamProject *project)
    : RamObject{shortName, name, ObjectType::ScheduleRow, project}
{
    construct();
    insertData("project", project->uuid() );
    createData();
}

RamProject *RamScheduleRow::project() const
{
    QString projUuid = getData("project").toString();
    if (projUuid == "")
        return nullptr;
    return RamProject::get( projUuid );
}

RamUser *RamScheduleRow::user() const
{
    QString userUuid = getData("user").toString();
    if (userUuid == "")
        return nullptr;
    return RamUser::get( userUuid );
}

void RamScheduleRow::setUser(RamObject *user)
{
    if (user) {
        insertData("user", user->uuid());
    }
    else {
        insertData("user", "");
    }
}

QString RamScheduleRow::iconName() const
{
    RamUser *u = user();
    if (u) return u->iconName();
    return RamObject::iconName();
}

QString RamScheduleRow::name() const
{
    QString n = RamObject::name();

    RamUser *u = user();
    if (u)
        return n + " | " + u->name();

    return n;
}

QColor RamScheduleRow::color() const
{
    RamUser *u = user();
    if (u)
        return u->color();

    return RamObject::color();
}

void RamScheduleRow::edit(bool show)
{
    if (!ui_editWidget)
        ui_editWidget = createEditFrame(new ScheduleRowEditWidget());

    if (show) showEdit( ui_editWidget );
}

RamScheduleRow::RamScheduleRow(const QString &uuid):
    RamObject(uuid, ObjectType::ScheduleRow)
{
    construct();
}

void RamScheduleRow::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/calendar";
    m_editRole = Lead;
}
