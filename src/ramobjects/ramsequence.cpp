#include "ramsequence.h"
#include "ramproject.h"
#include "sequenceeditwidget.h"

RamSequence::RamSequence(QString shortName, RamProject *project, QString name, QString uuid):
    RamObject(shortName, name, uuid, project)
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

void RamSequence::edit()
{
    if (!m_editReady)
    {
        SequenceEditWidget *w = new SequenceEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    showEdit();
}
