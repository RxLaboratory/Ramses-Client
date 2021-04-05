#include "ramshot.h"

RamShot::RamShot(QString shortName, QString name, QString sequenceUuid, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _sequenceUuid = sequenceUuid;
    _dbi->createShot(_shortName, _name, _sequenceUuid, _uuid);
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
    _sequenceUuid = sequenceUuid;
}

qreal RamShot::duration() const
{
    return _duration;
}

void RamShot::setDuration(const qreal &duration)
{
    _duration = duration;
    emit changed(this);
}

int RamShot::order() const
{
    return _order;
}

void RamShot::setOrder(int order)
{
    _order = order;
    if (!_dbi->isSuspended()) _orderChanged = true;
    emit changed(this);
}

void RamShot::update()
{
    _dbi->updateShot(_uuid, _shortName, _name, _sequenceUuid, _duration);
    if (_orderChanged)
    {
        _dbi->moveShot(_uuid, _order);
        _orderChanged = false;
    }
}