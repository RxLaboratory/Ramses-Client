#include "ramobject.h"

QMap<QString, RamObject*> RamObject::m_existingObjects = QMap<QString, RamObject*>();

RamObject::RamObject(QObject *parent) : QObject(parent)
{
    _removing = false;
    _shortName = "";
    _name = "";
    _uuid = RamUuid::generateUuidString(_shortName);
    _dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[_uuid] = this;
}

RamObject::RamObject(QString uuid, QObject *parent): QObject(parent)
{
    _removing = false;
    _shortName = "";
    _name = "";
    _uuid = uuid;
    _dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[_uuid] = this;
}

RamObject::RamObject(QString shortName, QString name, QString uuid, QObject *parent) : QObject(parent)
{
    _removing = false;
    _shortName = shortName;
    _name = name;
    if(_name == "" ) _name = shortName;
    if (uuid != "") _uuid = uuid;
    else _uuid = RamUuid::generateUuidString(_shortName);
    _dbi = DBInterface::instance();
    this->setObjectName( "RamObject" );
    m_existingObjects[_uuid] = this;
}

QString RamObject::shortName() const
{
    return _shortName;
}

void RamObject::setShortName(const QString &shortName)
{
    if (shortName == _shortName) return;
    _dirty  = true;
    _shortName = shortName;
    emit changed(this);
}

QString RamObject::name() const
{
    return _name;
}

void RamObject::setName(const QString &name)
{
    if (name == _name) return;
    _dirty = true;
    _name = name;
    emit changed(this);
}

QString RamObject::uuid() const
{
    return _uuid;
}

void RamObject::setUuid(const QString &uuid)
{
    if (uuid == _uuid) return;
    _uuid = uuid;
    emit changed(this);
}

void RamObject::remove()
{
    qDebug().noquote() << "Removing: " + _name + " (uuid: " + _uuid + ")";
    qDebug().noquote() << "- " + this->objectName();
    if (_removing) return;
    qDebug().noquote() << "> Accepted";

    _removing = true;
#ifdef QT_DEBUG
    dumpObjectInfo();
#endif
    emit removed(this);
    qDebug().noquote() << "> " + _name + " Removed";

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
    if (order == _order) return;
    _dirty = true;
    int previous = _order;
    _order = order;
    if (!_dbi->isSuspended()) _orderChanged = true;
    emit orderChanged(this, previous, order);
}

bool RamObject::is(RamObject *other)
{
    if (!other) return false;
    return other->uuid() == _uuid;
}

RamObject *RamObject::obj(QString uuid)
{
    RamObject *obj = m_existingObjects.value(uuid, nullptr );
    if (!obj) obj = new RamObject(uuid);
    return obj;
}

void RamObject::update()
{
    _dirty = false;
}
