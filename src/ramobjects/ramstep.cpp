#include "ramstep.h"
#include "ramproject.h"

RamStep::RamStep(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Step);
    m_project = nullptr;
    m_template = true;
    if (m_template) _dbi->createTemplateStep(_shortName, _name, _uuid);
    init();
}

RamStep::RamStep(QString shortName, QString name, RamProject *project, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Step);
    m_project = project;
    m_template = false;
    _dbi->createStep(_shortName, _name, m_project->uuid(), _uuid);
    init();
}

RamStep::~RamStep()
{
    if (m_template) _dbi->removeTemplateStep(_uuid);
    else _dbi->removeStep(_uuid);
}

void RamStep::init()
{
    m_type = AssetProduction;
    _order = 0;
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
    RamStep *step = new RamStep(_shortName, _name, projectUuid);
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
    _dirty = true;
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
    _dbi->assignUser(_uuid, u->uuid());
}

void RamStep::userUnassigned(RamObject *u)
{
    _dbi->unassignUser(_uuid, u->uuid());
}

RamObjectList *RamStep::applications() const
{
    return m_applications;
}

void RamStep::applicationAssigned(RamObject *a)
{
    _dbi->assignApplication(_uuid, a->uuid());
}

void RamStep::applicationUnassigned(RamObject *a)
{
    _dbi->unassignApplication(_uuid, a->uuid());
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
    if(!_dirty) return;
    RamObject::update();
    QString type = "asset";
    if (m_type == PostProduction) type = "post";
    else if (m_type == PreProduction) type = "pre";
    else if (m_type == ShotProduction) type = "shot";
    if (m_template) _dbi->updateTemplateStep(_uuid, _shortName, _name, type);
    else _dbi->updateStep(_uuid, _shortName, _name, type, _order);
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
