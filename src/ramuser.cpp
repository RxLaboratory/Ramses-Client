#include "ramuser.h"

RamUser::RamUser(QString shortName, QString name, QString uuid, QString path, QObject *parent) :
    RamObject(shortName, name, uuid, path, parent)
{
    _role = Standard;
    _dbi->createUser(_shortName, _name, _uuid);
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

void RamUser::update()
{
    QString role = "standard";
    if (_role == Admin) role = "admin";
    else if (_role == Lead) role = "lead";
    _dbi->updateUser(_uuid, _shortName, _name, role, _folderPath);
}

void RamUser::updatePassword(QString c, QString n)
{
    _dbi->updateUserPassword(_uuid, c, n);
}
