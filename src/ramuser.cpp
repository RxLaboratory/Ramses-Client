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
}
