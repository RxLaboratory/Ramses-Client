#include "ramuser.h"

#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramstep.h"
#include "usereditwidget.h"
#include "ramdatainterface/dbinterface.h"

// KEYS //

const QString RamUser::ENUMVALUE_Admin = QStringLiteral("admin");
const QString RamUser::ENUMVALUE_ProjectAdmin = QStringLiteral("project");
const QString RamUser::ENUMVALUE_Lead = QStringLiteral("lead");
const QString RamUser::ENUMVALUE_Standard = QStringLiteral("standard");

// STATIC //

QFrame *RamUser::ui_editWidget = nullptr;

QHash<QString, RamUser*> RamUser::m_existingObjects = QHash<QString, RamUser*>();

RamUser *RamUser::get(QString uuid )
{
    if (!checkUuid(uuid, User)) return nullptr;

    RamUser *u = m_existingObjects.value(uuid);
    if (u) return u;

    // Finally return a new instance
    return new RamUser(uuid);
}

RamUser *RamUser::c(RamObject *o)
{
    //return qobject_cast<RamUser*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamUser*>(o);
}

// PUBLIC //

RamUser::RamUser(QString shortName, QString name) :
    RamObject(shortName, name, User, nullptr, shortName == "Ramses")
{
    construct();
    if (shortName == "Ramses") m_uuid = "none";
    createData();
}

RamUser::RamUser(QString uuid):
    RamObject(uuid, User, nullptr)
{
    construct();
}

RamUser::UserRole RamUser::role() const
{
    // Always an admin if this is not a team project
    if (!DBInterface::instance()->isTeamProject())
        return Admin;

    if (m_role == ENUMVALUE_Admin) return Admin;
    if (m_role == ENUMVALUE_ProjectAdmin) return ProjectAdmin;
    if (m_role == ENUMVALUE_Lead) return Lead;
    return Standard;
}

bool RamUser::setRole(const UserRole &role)
{
    switch(role)
    {
    case Admin:
        return setRole( ENUMVALUE_Admin );
    case ProjectAdmin:
        return setRole( ENUMVALUE_ProjectAdmin );
    case Lead:
        return setRole( ENUMVALUE_Lead );
    case Standard:
        return setRole( ENUMVALUE_Standard );
    }

    return false;
}

bool RamUser::setRole(const QString role)
{
    if (DBInterface::instance()->setUserRole(m_uuid, role)) {
        emitDataChanged();
        return true;
    }

    return false;
}

DBTableModel *RamUser::schedule() const
{
    m_schedule->load();
    return m_schedule;
}

bool RamUser::isStepAssigned(RamStep *step) const
{
    m_schedule->load();
    // Check in schedule
    for(int i = 0; i < m_schedule->rowCount(); i++)
    {
        RamScheduleEntry *entry = RamScheduleEntry::c( m_schedule->get(i) );
        if (!entry) continue;

        if (step->is(entry->step())) return true;
    }

    if (step->type() != RamStep::ShotProduction && step->type() != RamStep::AssetProduction) return false;

    // Check in status
    RamProject *proj = step->project();
    if (!proj) return false;

    return proj->isUserAssigned(const_cast<RamUser*>(this), step);
}

QString RamUser::iconName() const
{
    return ":/icons/user";
}

QString RamUser::details() const
{
    switch(role())
    {
    case Admin:
        return "Administrator";
    case ProjectAdmin:
        return "Project Admin";
    case Lead:
        return "Lead";
    case Standard:
        return "Standard User";
    }
    return "User";
}

// PUBLIC SLOTS //

void RamUser::edit(bool show)
{
    if (!ui_editWidget) ui_editWidget = createEditFrame(new UserEditWidget(this));

    if (show) showEdit( ui_editWidget  );
}

// PROTECTED //

QString RamUser::folderPath() const
{
    return Ramses::instance()->path(RamObject::UsersFolder) + "/" + shortName();
}

// PRIVATE //

void RamUser::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/user";
    m_editRole = Admin;
    m_role = DBInterface::instance()->getUserRole(m_uuid);
    //m_schedule = createModel(RamObject::ScheduleEntry, "schedule");
    m_schedule = new RamScheduleEntryModel();
    m_schedule->addFilterValue( "user", this->uuid() );
    m_schedule->addLookUpKey("date");
    m_schedule->addLookUpKey("step");
    m_schedule->addLookUpKey("project");
}
