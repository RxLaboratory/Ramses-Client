#include "ramuser.h"

RamUser::RamUser(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(User);
    _role = Standard;
    m_dbi->createUser(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamUser" );
}

RamUser::~RamUser()
{
    m_dbi->removeUser(m_uuid);
}

RamUser::UserRole RamUser::role() const
{
    return _role;
}

void RamUser::setRole(const UserRole &role)
{
    if (role == _role) return;
    m_dirty = true;
    _role = role;
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
    return _folderPath;
}

void RamUser::setFolderPath(const QString &folderPath)
{
    if (folderPath == _folderPath) return;
    m_dirty = true;
    _folderPath = folderPath;
    emit changed(this);
}

void RamUser::update()
{
    if(!m_dirty) return;
    RamObject::update();
    QString role = "standard";
    if (_role == Admin) role = "admin";
    else if (_role == ProjectAdmin) role = "project";
    else if (_role == Lead) role = "lead";

    QString path = _folderPath;
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
