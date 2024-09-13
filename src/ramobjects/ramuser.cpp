#include "ramuser.h"

#include "ramses.h"
#include "ramscheduleentry.h"
#include "ramstep.h"
#include "ramdatainterface/dbinterface.h"

// KEYS //

const QString RamUser::ENUMVALUE_Admin = QStringLiteral("admin");
const QString RamUser::ENUMVALUE_ProjectAdmin = QStringLiteral("project");
const QString RamUser::ENUMVALUE_Lead = QStringLiteral("lead");
const QString RamUser::ENUMVALUE_Standard = QStringLiteral("standard");

// STATIC //

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

QJsonObject RamUser::toJson() const
{
    QJsonObject obj = RamObject::toJson();

    obj.insert("role", m_role );

    return obj;
}

void RamUser::loadJson(const QJsonObject &obj)
{
    beginLoadJson(obj);
    setRole(obj.value("role").toString("standard"));
    endLoadJson();
}

RamUser::RamUser(QString uuid):
    RamObject(uuid, User, nullptr)
{
    construct();
}

RamUser::UserRole RamUser::role() const
{
    // Always an admin if this is not a team project
    if (!DBInterface::i()->isTeamProject())
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
    if (DBInterface::i()->setUserRole(m_uuid, role)) {
        m_role = role;
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
    if (!ui_jsonEditWidget) {
        ui_jsonEditWidget = new RamJsonUserEditWidget(
            this == Ramses::i()->currentUser(),
            Ramses::i()->isAdmin(),
            DBInterface::i()->isTeamProject(),
            m_uuid);
        connect(ui_jsonEditWidget, &RamJsonUserEditWidget::dataEdited,
                this, &RamUser::loadJson);
    }

    if (show) showEdit( ui_jsonEditWidget );
}

// PROTECTED //

QString RamUser::folderPath() const
{
    return Ramses::i()->path(RamObject::UsersFolder) + "/" + shortName();
}

// PRIVATE //

void RamUser::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/user";
    m_editRole = Admin;
    m_role = DBInterface::i()->getUserRole(m_uuid);
    //m_schedule = createModel(RamObject::ScheduleEntry, "schedule");
    m_schedule = new RamScheduleEntryModel();
    m_schedule->addFilterValue( "user", this->uuid() );
    m_schedule->addLookUpKey("date");
    m_schedule->addLookUpKey("step");
    m_schedule->addLookUpKey("project");
}
