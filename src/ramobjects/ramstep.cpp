#include "ramstep.h"

RamStep::RamStep(QString shortName, QString name, bool tplt, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _projectUuid = "";
    _template = tplt;
    _type = AssetProduction;
    _order = 0;
    if (_template) _dbi->createTemplateStep(_shortName, _name, _uuid);
}

RamStep::RamStep(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _projectUuid = projectUuid;
    _template = false;
    _type = AssetProduction;
    _order = 0;
    _dbi->createStep(_shortName, _name, projectUuid, _uuid);
}

RamStep::~RamStep()
{
    if (_template) _dbi->removeTemplateStep(_uuid);
    else _dbi->removeStep(_uuid);
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

QList<RamUser *> RamStep::users() const
{
    return _users;
}

void RamStep::clearUsers()
{
    _users.clear();
}

void RamStep::assignUser(RamObject *obj)
{
    RamUser *user = (RamUser*)obj;
    _users << user;
    _dbi->assignUser(_uuid, user->uuid());
    connect(user, &RamUser::removed, this, &RamStep::userRemoved);
    emit userAssigned(user);
}

void RamStep::unassignUser(RamObject *user)
{
    unassignUser( user->uuid() );
}

void RamStep::unassignUser(QString uuid)
{
    _dbi->unassignUser(_uuid, uuid);
    for (int i = _users.count() -1; i >= 0; i--)
    {
        RamUser *u = _users[i];
        if (u->uuid() == uuid)
        {
            _users.removeAt(i);
            emit userUnassigned(uuid);
        }
    }
}

QList<RamApplication *> RamStep::applications() const
{
    return _applications;
}

void RamStep::clearApplications()
{
    _applications.clear();
}

void RamStep::assignApplication(RamApplication *app)
{
    _applications << app;
    _dbi->assignApplication(_uuid, app->uuid());
    connect(app, &RamApplication::removed, this, &RamStep::applicationRemoved);
    emit applicationAssigned(app);
}

void RamStep::unassignApplication(RamObject *app)
{
    unassignApplication( app->uuid() );
}

void RamStep::unassignApplication(QString uuid)
{
    _dbi->unassignApplication(_uuid, uuid);
    for (int i = _applications.count() -1; i >= 0; i--)
    {
        RamApplication *a = _applications[i];
        if (a->uuid() == uuid)
        {
            _applications.removeAt(i);
            emit applicationUnassigned(uuid);
        }
    }
}

QList<RamFileType *> RamStep::inputFileTypes()
{
    QList<RamFileType *> fts;

    for (RamApplication *app: qAsConst(_applications))
    {
        fts.append( app->importFileTypes() );
        fts.append( app->nativeFileTypes() );
    }

    return fts;
}

QList<RamFileType *> RamStep::outputFileTypes()
{
    QList<RamFileType *> fts;

    for (RamApplication *app: qAsConst(_applications))
    {
        fts.append( app->exportFileTypes() );
        fts.append( app->nativeFileTypes() );
    }

    return fts;
}

void RamStep::update()
{
    QString type = "asset";
    if (_type == PostProduction) type = "post";
    else if (_type == PreProduction) type = "pre";
    else if (_type == ShotProduction) type = "shot";
    if (_template) _dbi->updateTemplateStep(_uuid, _shortName, _name, type);
    else _dbi->updateStep(_uuid, _shortName, _name, type, _order);
}

void RamStep::userRemoved(RamObject *o)
{
    unassignUser(o);
}

void RamStep::applicationRemoved(RamObject *o)
{
    unassignApplication(o->uuid());
}

int RamStep::order() const
{
    return _order;
}

void RamStep::setOrder(int order)
{
    _order = order;
    emit changed(this);
}

QString RamStep::projectUuid() const
{
    return _projectUuid;
}

void RamStep::setProjectUuid(const QString &projectUuid)
{
    _projectUuid = projectUuid;
}
