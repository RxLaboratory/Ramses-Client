#include "ramuser.h"

RamUser::RamUser(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _role = Standard;
    _dbi->createUser(_shortName, _name, _uuid);
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
    _role = role;
    emit changed();
}

QString RamUser::folderPath() const
{
    return _folderPath;
}

void RamUser::setFolderPath(const QString &folderPath)
{
    _folderPath = folderPath;
    emit changed();
}

void RamUser::update()
{
    QString role = "standard";
    if (_role == Admin) role = "admin";
    else if (_role == Lead) role = "lead";

    QString path = _folderPath;
    if (path == "") path = "auto";

    _dbi->updateUser(_uuid, _shortName, _name, role, path);
}

void RamUser::updatePassword(QString c, QString n)
{
    _dbi->updateUserPassword(_uuid, c, n);
}
