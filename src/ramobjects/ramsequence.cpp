#include "ramsequence.h"
#include "ramproject.h"

RamSequence::RamSequence(QString shortName, RamProject *project, QString name, QString uuid, QObject *parent):
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(Sequence);
    m_project = project;
    m_dbi->createSequence(m_shortName, m_name, m_project->uuid(), m_uuid);

    this->setObjectName( "RamSequence" );
}

RamSequence::~RamSequence()
{
    m_dbi->removeSequence(m_uuid);
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
    if(!m_dirty) return;
    RamObject::update();
    m_dbi->updateSequence(m_uuid, m_shortName, m_name);
}

void RamSequence::remove()
{
    if (m_removing) return;
    for(int i = m_objectsList.count()-1; i >= 0 ; i--)
    {
        m_objectsList.at(i)->remove();
    }
    RamObjectList::remove();
}
