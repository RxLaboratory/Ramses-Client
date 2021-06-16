#include "ramsequence.h"
#include "ramproject.h"

RamSequence::RamSequence(QString shortName, RamProject *project, QString name, QString uuid, QObject *parent):
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(Sequence);
    m_project = project;
    _dbi->createSequence(_shortName, _name, m_project->uuid(), _uuid);

    this->setObjectName( "RamSequence" );
}

RamSequence::~RamSequence()
{
    _dbi->removeSequence(_uuid);
}

RamProject *RamSequence::project() const
{
    return m_project;
}

void RamSequence::append(RamShot *shot)
{
    shot->setSequenceUuid( _uuid );
    shot->update();
    RamObjectList::append(shot);
}

void RamSequence::createShot(QString shortName, QString name)
{
    RamShot *shot = new RamShot(shortName, m_project, name, _uuid);
    append(shot);
}

void RamSequence::update()
{
    if(!_dirty) return;
    RamObject::update();
    _dbi->updateSequence(_uuid, _shortName, _name);
}
