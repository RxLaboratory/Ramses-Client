#include "ramuser.h"

RamUser::RamUser(QString shortName, QString name, QString uuid, QString path, QObject *parent) :
    RamObject(shortName, name, uuid, path, parent)
{

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
    _dbi->updateUser(_uuid, _shortName, _name);
}

void RamUser::updatePassword(QString c, QString n)
{
    _dbi->updateUserPassword(_uuid, c, n);
}
