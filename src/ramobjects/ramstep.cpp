#include "ramstep.h"

RamStep::RamStep(QString shortName, QString name, bool tplt, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Step);
    _projectUuid = "";
    _template = tplt;
    if (_template) _dbi->createTemplateStep(_shortName, _name, _uuid);
    init();
}

RamStep::RamStep(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Step);
    _projectUuid = projectUuid;
    _template = false;
    _dbi->createStep(_shortName, _name, projectUuid, _uuid);
    init();
}

RamStep::~RamStep()
{
    if (_template) _dbi->removeTemplateStep(_uuid);
    else _dbi->removeStep(_uuid);
}

void RamStep::init()
{
    _type = AssetProduction;
    _order = 0;
    _users = new RamObjectList();
    _applications = new RamObjectList();
    connect(_users, &RamObjectList::objectRemoved, this, &RamStep::userAssigned);
    connect(_users, &RamObjectList::objectRemoved, this, &RamStep::userUnassigned);
    connect(_applications, &RamObjectList::objectRemoved, this, &RamStep::applicationAssigned);
    connect(_applications, &RamObjectList::objectRemoved, this, &RamStep::applicationUnassigned);
}

bool RamStep::isTemplate() const
{
    return _template;
}

RamStep* RamStep::createFromTemplate(QString projectUuid)
{
    // Create
    RamStep *step = new RamStep(_shortName, _name, projectUuid);
    step->setType(_type);
    // and update
    step->update();
    return step;
}

RamStep::Type RamStep::type() const
{
    return _type;
}

void RamStep::setType(const Type &type)
{
    if (type == _type) return;
    _dirty = true;
    _type = type;
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
    return _users;
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
    return _applications;
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

    for ( int i = 0; i < _applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( _applications->at(i) );
        fts.append( app->importFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

QList<RamObject *> RamStep::outputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < _applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( _applications->at(i) );
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
    if (_type == PostProduction) type = "post";
    else if (_type == PreProduction) type = "pre";
    else if (_type == ShotProduction) type = "shot";
    if (_template) _dbi->updateTemplateStep(_uuid, _shortName, _name, type);
    else _dbi->updateStep(_uuid, _shortName, _name, type, _order);
}

QString RamStep::projectUuid() const
{
    return _projectUuid;
}

void RamStep::setProjectUuid(const QString &projectUuid)
{
    _projectUuid = projectUuid;
}
