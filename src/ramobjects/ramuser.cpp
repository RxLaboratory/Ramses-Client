#include "ramuser.h"

RamUser::RamUser(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(User);
    _role = Standard;
    _dbi->createUser(_shortName, _name, _uuid);

    this->setObjectName( "RamUser" );
}

RamUser::~RamUser()
{
    _dbi->removeUser(_uuid);
}

RamUser::UserRole RamUser::role() const
{
    return _role;
}

void RamUser::setRole(const UserRole &role)
{
    if (role == _role) return;
    _dirty = true;
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
    _dirty = true;
    _folderPath = folderPath;
    emit changed(this);
}

void RamUser::update()
{
    if(!_dirty) return;
    RamObject::update();
    QString role = "standard";
    if (_role == Admin) role = "admin";
    else if (_role == ProjectAdmin) role = "project";
    else if (_role == Lead) role = "lead";

    QString path = _folderPath;
    if (path == "") path = "auto";

    _dbi->updateUser(_uuid, _shortName, _name, role, path);
}

void RamUser::updatePassword(QString c, QString n)
{
    _dbi->updateUserPassword(_uuid, c, n);
}

RamUser *RamUser::user(QString uuid)
{
    return qobject_cast<RamUser*>( RamObject::obj(uuid) );
}
