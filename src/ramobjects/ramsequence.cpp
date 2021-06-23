#include "ramsequence.h"
#include "ramproject.h"
#include "sequenceeditwidget.h"

RamSequence::RamSequence(QString shortName, RamProject *project, QString name, QString uuid):
    RamObject(shortName, name, uuid, project)
{
    this->setObjectType(Sequence);
    m_project = project;
    m_dbi->createSequence(m_shortName, m_name, m_project->uuid(), m_uuid);

    m_shots = new RamObjectFilterModel(this);
    m_shots->setSourceModel( project->shots() );
    m_shots->setFilterUuid( m_uuid );

    this->setObjectName( "RamSequence" );
}

RamSequence::~RamSequence()
{
    m_dbi->removeSequence(m_uuid);
}

int RamSequence::shotCount() const
{
    return m_shots->rowCount();
}

double RamSequence::duration() const
{
    double duration = 0;
    for (int i = 0; i < m_shots->rowCount(); i++)
    {
        quintptr iptr = m_shots->data( m_shots->index(i,0), Qt::UserRole).toULongLong();
        RamShot *shot = reinterpret_cast<RamShot*>( iptr );
        duration += shot->duration();
    }
    return duration;
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

void RamSequence::edit(bool show)
{
    if (!m_editReady)
    {
        SequenceEditWidget *w = new SequenceEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    showEdit(show);
}
