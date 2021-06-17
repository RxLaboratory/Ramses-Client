#include "ramstep.h"
#include "ramproject.h"
#include "ramses.h"

RamStep::RamStep(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(Step);
    m_project = nullptr;
    m_template = true;
    if (m_template) m_dbi->createTemplateStep(m_shortName, m_name, m_uuid);
    init();
}

RamStep::RamStep(QString shortName, QString name, RamProject *project, QString uuid):
    RamObject(shortName, name, uuid, project)
{
    setObjectType(Step);
    m_project = project;
    m_template = false;
    m_dbi->createStep(m_shortName, m_name, m_project->uuid(), m_uuid);
    init();
}

RamStep::~RamStep()
{
    if (m_template) m_dbi->removeTemplateStep(m_uuid);
    else m_dbi->removeStep(m_uuid);
}

void RamStep::init()
{
    m_type = AssetProduction;
    m_order = 0;
    m_users = new RamObjectList();
    m_applications = new RamObjectList();
    connect(m_users, &RamObjectList::objectAdded, this, &RamStep::userAssigned);
    connect(m_users, &RamObjectList::objectRemoved, this, &RamStep::userUnassigned);
    connect(m_applications, &RamObjectList::objectAdded, this, &RamStep::applicationAssigned);
    connect(m_applications, &RamObjectList::objectRemoved, this, &RamStep::applicationUnassigned);

    this->setObjectName( "RamStep" );
}

bool RamStep::isTemplate() const
{
    return m_template;
}

RamStep* RamStep::createFromTemplate(QString projectUuid)
{
    // Create
    RamStep *step = new RamStep(m_shortName, m_name, projectUuid);
    step->setType(m_type);
    // and update
    step->update();
    return step;
}

RamStep::Type RamStep::type() const
{
    return m_type;
}

void RamStep::setType(const Type &type)
{
    if (type == m_type) return;
    m_dirty = true;
    m_type = type;
    emit changed(this);
}

void RamStep::setType(QString type)
{
    if (type == "pre") setType(PreProduction);
    else if (type == "asset") setType(AssetProduction);
    else if (type == "shot") setType(ShotProduction);
    else if (type == "post") setType(PostProduction);
}

RamObjectList *RamStep::users() const
{
    return m_users;
}

void RamStep::userAssigned(RamObject *u)
{
    m_dbi->assignUser(m_uuid, u->uuid());
}

void RamStep::userUnassigned(RamObject *u)
{
    m_dbi->unassignUser(m_uuid, u->uuid());
}

RamObjectList *RamStep::applications() const
{
    return m_applications;
}

void RamStep::applicationAssigned(RamObject *a)
{
    m_dbi->assignApplication(m_uuid, a->uuid());
}

void RamStep::applicationUnassigned(RamObject *a)
{
    m_dbi->unassignApplication(m_uuid, a->uuid());
}

QList<RamObject *> RamStep::inputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( m_applications->at(i) );
        fts.append( app->importFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

QList<RamObject *> RamStep::outputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( m_applications->at(i) );
        fts.append( app->exportFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

void RamStep::update()
{
    if(!m_dirty) return;
    RamObject::update();
    QString type = "asset";
    if (m_type == PostProduction) type = "post";
    else if (m_type == PreProduction) type = "pre";
    else if (m_type == ShotProduction) type = "shot";
    if (m_template) m_dbi->updateTemplateStep(m_uuid, m_shortName, m_name, type);
    else m_dbi->updateStep(m_uuid, m_shortName, m_name, type, m_order);
}

RamStep *RamStep::step(QString uuid)
{
    return qobject_cast<RamStep*>( RamObject::obj(uuid) );
}

RamProject *RamStep::project() const
{
    return m_project;
}

void RamStep::setProject( RamProject *project )
{
    m_project = project;
}
