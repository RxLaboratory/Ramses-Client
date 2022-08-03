#include "ramuser.h"

#include "ramses.h"
#include "usereditwidget.h"
#include "ramdatainterface/dbinterface.h"
#include "duqf-utils/simplecrypt.h"

// STATIC //

RamUser *RamUser::getObject(QString uuid, bool constructNew )
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamUser( uuid );
    return qobject_cast<RamUser*>( obj );
}

// PUBLIC //

RamUser::RamUser(QString shortName, QString name) :
    RamObject(shortName, name, User)
{
    construct();

    m_schedule = new RamObjectList<RamScheduleEntry*>(shortName + "-schdl", name + " | Schedule", this);
    insertData("schedule", m_schedule->uuid());
}

RamUser::UserRole RamUser::role() const
{
    QString roleStr = getData("role").toString("standard");
    if (roleStr == "admin") return Admin;
    if (roleStr == "project") return ProjectAdmin;
    if (roleStr == "lead") return Lead;
    return Standard;
}

void RamUser::setRole(const UserRole &role)
{
    switch(role)
    {
    case Admin:
        insertData("role", "admin");
        break;
    case ProjectAdmin:
        insertData("role", "project");
        break;
    case Lead:
        insertData("role", "lead");
        break;
    case Standard:
        insertData("role", "standard");
        break;
    }
}

void RamUser::setRole(const QString role)
{
    insertData("role", role);
}

QColor RamUser::color() const
{
    return QColor( getData("color").toString("#434343") );
}

void RamUser::setColor(const QColor &newColor)
{
    insertData("color", newColor.name());
}

RamObjectList<RamScheduleEntry*> *RamUser::schedule() const
{
    return m_schedule;
}

bool RamUser::isStepAssigned(RamStep *step) const
{
    // Check in schedule
    for(int i = 0; i < m_schedule->rowCount(); i++)
    {
        RamScheduleEntry *entry = m_schedule->at(i);
        if (!entry) continue;

        if (step->is(entry->step())) return true;
    }

    if (step->type() != RamStep::ShotProduction && step->type() != RamStep::AssetProduction) return false;

    // Check in status
    RamObjectList<RamItem*> *items;
    if (step->type() == RamStep::ShotProduction) items = step->project()->shots();
    else items = step->project()->assets();

    for (int i =0; i < items->rowCount(); i++)
    {
        RamItem *item = items->at(i);
        RamStatus *status = item->status(step);
        if (!status) continue;
        if (this->is(status->assignedUser())) return true;
    }

    return false;
}

void RamUser::updatePassword(QString c, QString n)
{
    QString prev = getData("password").toString();

    // Hash passwords
    c = DBInterface::instance()->generatePassHash(c);
    n = DBInterface::instance()->generatePassHash(n);

    if (c == prev) insertData("password", n);
}

// PUBLIC SLOTS //

void RamUser::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new UserEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamUser::RamUser(QString uuid):
    RamObject(uuid, State)
{
    construct();

    m_schedule = RamObjectList<RamScheduleEntry*>::getObject( getData("schedule").toString(), true);
}

QString RamUser::folderPath() const
{
    return Ramses::instance()->path(RamObject::UsersFolder) + "/" + shortName();
}

QString RamUser::dataString() const
{
    QString dataStr = RamAbstractObject::dataString();
    if (dataStr == "") return "";
    // Decrypt
    SimpleCrypt crypto( SimpleCrypt::clientKey(CLIENT_BUILD_KEY) );
    dataStr = crypto.decryptToString(dataStr);
    return dataStr;
}

void RamUser::setDataString(QString data)
{
    // Encrypt
    SimpleCrypt crypto( SimpleCrypt::clientKey(CLIENT_BUILD_KEY) );
    data = crypto.encryptToString(data);
    // Set
    RamAbstractObject::setDataString(data);
}

void RamUser::createData(QString data)
{
    // Encrypt
    SimpleCrypt crypto( SimpleCrypt::clientKey(CLIENT_BUILD_KEY) );
    data = crypto.encryptToString(data);
    // Set
    RamAbstractObject::createData(data);
}

// PRIVATE //

void RamUser::construct()
{
    m_icon = ":/icons/user";
    m_editRole = Admin;
}
