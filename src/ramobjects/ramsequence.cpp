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
    shot->setSequence( this );
    shot->update();
    RamObjectList::append(shot);
}

void RamSequence::createShot(QString shortName, QString name)
{
    RamShot *shot = new RamShot(shortName, m_project, this, name, "");
    append(shot);
}

RamSequence *RamSequence::sequence(QString uuid)
{
    return qobject_cast<RamSequence*>( RamObject::obj(uuid) );
}

void RamSequence::update()
{
    if(!_dirty) return;
    RamObject::update();
    _dbi->updateSequence(_uuid, _shortName, _name);
}

void RamSequence::remove()
{
    if (_removing) return;
    _removing = true;
    for(int i = m_objectsList.count(); i >= 0 ; i--)
    {
        m_objectsList.at(i)->remove();
    }
    RamObjectList::remove();
}
