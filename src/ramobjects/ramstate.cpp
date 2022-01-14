#include "ramstate.h"

#include "ramses.h"

RamState::RamState(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    m_icon = ":/icons/state-l";
    m_editRole = Admin;

    this->setObjectType(State);
    m_dbi->createState(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamState" );
}

RamState::~RamState()
{

}

QColor RamState::color() const
{
    return _color;
}

void RamState::setColor(const QColor &color)
{
    if (_color == color) return;
    m_dirty = true;
    _color = color;
    emit changed(this);
}

int RamState::completionRatio() const
{
    return _completionRatio;
}

void RamState::setCompletionRatio(int completionRatio)
{
    if (_completionRatio == completionRatio) return;
    m_dirty = true;
    _completionRatio = completionRatio;
    emit changed(this);
}

void RamState::update()
{
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateState(m_uuid, m_shortName, m_name, _color.name(), QString::number(_completionRatio), m_comment);
}

void RamState::removeFromDB()
{
    m_dbi->removeState(m_uuid);
}

RamState *RamState::state(QString uuid)
{
    return qobject_cast<RamState*>( RamObject::obj(uuid) );
}
