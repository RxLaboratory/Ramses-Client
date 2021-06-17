#include "ramshot.h"
#include "ramsequence.h"

RamShot::RamShot(QString shortName, RamProject *project, RamSequence *sequence, QString name, QString uuid):
    RamItem(shortName, project, name, uuid, sequence)
{
    setObjectType(Shot);
    setProductionType(RamStep::ShotProduction);
    m_sequence = sequence;
    _dbi->createShot(_shortName, _name, m_sequence->uuid(), _uuid);

    this->setObjectName( "RamShot " + _shortName );
}

RamShot::~RamShot()
{
    _dbi->removeShot(_uuid);
}

RamSequence *RamShot::sequence() const
{
    return m_sequence;
}

void RamShot::setSequence(RamSequence *sequence)
{
    if (m_sequence->is(sequence)) return;
    _dirty = true;
    setParent(sequence);
    m_sequence = sequence;
    emit changed(this);
}

qreal RamShot::duration() const
{
    return m_duration;
}

void RamShot::setDuration(const qreal &duration)
{
    if (m_duration == duration) return;
    _dirty = true;
    m_duration = duration;
    emit changed(this);
}

RamShot *RamShot::shot(QString uuid)
{
    return qobject_cast<RamShot*>( RamObject::obj(uuid) );
}

void RamShot::update()
{
    if(!_dirty) return;
    RamObject::update();
    _dbi->updateShot(_uuid, _shortName, _name, m_sequence->uuid(), m_duration);
    if (_orderChanged)
    {
        _dbi->moveShot(_uuid, _order);
        _orderChanged = false;
    }
}
