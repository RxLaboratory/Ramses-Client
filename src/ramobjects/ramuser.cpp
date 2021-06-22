#include "ramuser.h"

#include "ramses.h"
#include "usereditwidget.h"

RamUser::RamUser(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(User);
    m_role = Standard;
    m_dbi->createUser(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamUser" );

    // Settings file
    QString settingsPath = Ramses::instance()->configPath(this) % "/" % "ramses.ini";
    m_userSettings = new QSettings(settingsPath,  QSettings::IniFormat, this);
}

RamUser::~RamUser()
{
    m_dbi->removeUser(m_uuid);
}

RamUser::UserRole RamUser::role() const
{
    return m_role;
}

void RamUser::setRole(const UserRole &role)
{
    if (role == m_role) return;
    m_dirty = true;
    m_role = role;
    emit changed(this);
}

void RamUser::setRole(const QString role)
{
    if (role == "admin")setRole(Admin);
    else if (role == "project") setRole(ProjectAdmin);
    else if (role == "lead") setRole(Lead);
    else setRole(Standard);
}

QString RamUser::folderPath() const
{
    return m_folderPath;
}

void RamUser::setFolderPath(const QString &folderPath)
{
    if (folderPath == m_folderPath) return;
    m_dirty = true;
    m_folderPath = folderPath;
    emit changed(this);
}

void RamUser::update()
{
    if(!m_dirty) return;
    RamObject::update();
    QString role = "standard";
    if (m_role == Admin) role = "admin";
    else if (m_role == ProjectAdmin) role = "project";
    else if (m_role == Lead) role = "lead";

    QString path = m_folderPath;
    if (path == "") path = "auto";

    m_dbi->updateUser(m_uuid, m_shortName, m_name, role, path);
}

void RamUser::updatePassword(QString c, QString n)
{
    m_dbi->updateUserPassword(m_uuid, c, n);
}

RamUser *RamUser::user(QString uuid)
{
    return qobject_cast<RamUser*>( RamObject::obj(uuid) );
}

void RamUser::edit()
{
    if (!m_editReady)
    {
        UserEditWidget *w = new UserEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    showEdit();
}

QSettings *RamUser::userSettings() const
{
    return m_userSettings;
}
