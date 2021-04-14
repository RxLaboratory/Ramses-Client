#include "ramobject.h"


RamObject::RamObject(QString shortName, QString name, QString uuid, QObject *parent) : QObject(parent)
{
    _removing = false;
    _shortName = shortName;
    _name = name;
    if(_name == "" ) _name = shortName;
    if (uuid != "") _uuid = uuid;
    else _uuid = RamUuid::generateUuidString(_shortName);
    _dbi = DBInterface::instance();
}

QString RamObject::shortName() const
{
    return _shortName;
}

void RamObject::setShortName(const QString &shortName)
{
    _shortName = shortName;
    emit changed(this);
}

QString RamObject::name() const
{
    return _name;
}

void RamObject::setName(const QString &name)
{
    _name = name;
    emit changed(this);
}

QString RamObject::uuid() const
{
    return _uuid;
}

void RamObject::setUuid(const QString &uuid)
{
    _uuid = uuid;
    emit changed(this);
}

void RamObject::remove()
{
    qDebug() << "Removing RamObject: " + _name;
    if (_removing) return;
    qDebug() << "> Accepted";

    _removing = true;

    emit removed(this);
    this->deleteLater();
}

RamObject::ObjectType RamObject::objectType() const
{
    return _objectType;
}

void RamObject::setObjectType(RamObject::ObjectType type)
{
    _objectType = type;
}

int RamObject::order() const
{
    return _order;
}

void RamObject::setOrder(int order)
{
    int previous = _order;
    _order = order;
    if (!_dbi->isSuspended()) _orderChanged = true;
    emit orderChanged(this, previous, order);
}
