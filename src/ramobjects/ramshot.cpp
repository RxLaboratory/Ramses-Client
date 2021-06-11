#include "ramshot.h"

RamShot::RamShot(QString shortName, QString name, QString sequenceUuid, QString uuid, QObject *parent):
    RamItem(shortName, name, uuid, parent)
{
    setObjectType(Shot);
    _sequenceUuid = sequenceUuid;
    _dbi->createShot(_shortName, _name, _sequenceUuid, _uuid);

    this->setObjectName( "RamShot" );
}

RamShot::~RamShot()
{
    _dbi->removeShot(_uuid);
}

QString RamShot::sequenceUuid() const
{
    return _sequenceUuid;
}

void RamShot::setSequenceUuid(const QString &sequenceUuid)
{
    if (sequenceUuid == _sequenceUuid) return;
    _dirty = true;
    _sequenceUuid = sequenceUuid;
    emit changed(this);
}

qreal RamShot::duration() const
{
    return _duration;
}

void RamShot::setDuration(const qreal &duration)
{
    if (_duration == duration) return;
    _dirty = true;
    _duration = duration;
    emit changed(this);
}

void RamShot::update()
{
    if(!_dirty) return;
    RamObject::update();
    _dbi->updateShot(_uuid, _shortName, _name, _sequenceUuid, _duration);
    if (_orderChanged)
    {
        _dbi->moveShot(_uuid, _order);
        _orderChanged = false;
    }
}
