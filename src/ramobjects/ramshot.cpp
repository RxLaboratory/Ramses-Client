#include "ramshot.h"
#include "ramsequence.h"

RamShot::RamShot(QString shortName, RamProject *project, RamSequence *sequence, QString name, QString uuid):
    RamItem(shortName, project, name, uuid, sequence)
{
    setObjectType(Shot);
    setProductionType(RamStep::ShotProduction);
    m_sequence = sequence;
    m_dbi->createShot(m_shortName, m_name, m_sequence->uuid(), m_uuid);

    this->setObjectName( "RamShot " + m_shortName );
}

RamShot::~RamShot()
{
    m_dbi->removeShot(m_uuid);
}

RamSequence *RamShot::sequence() const
{
    return m_sequence;
}

void RamShot::setSequence(RamSequence *sequence)
{
    if (m_sequence->is(sequence)) return;
    m_dirty = true;
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
    m_dirty = true;
    m_duration = duration;
    emit changed(this);
}

RamShot *RamShot::shot(QString uuid)
{
    return qobject_cast<RamShot*>( RamObject::obj(uuid) );
}

void RamShot::update()
{
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateShot(m_uuid, m_shortName, m_name, m_sequence->uuid(), m_duration);
    if (m_orderChanged)
    {
        m_dbi->moveShot(m_uuid, m_order);
        m_orderChanged = false;
    }
}
