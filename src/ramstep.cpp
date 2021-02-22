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
    emit changed();
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

void RamStep::assignUser(RamUser *user)
{
    _users << user;
    _dbi->assignUser(_uuid, user->uuid());
    connect(user, &RamUser::destroyed, this, &RamStep::userDestroyed);
    emit newUser(user);
}

void RamStep::removeUser(RamUser *user)
{
    removeUser( user->uuid() );
}

void RamStep::removeUser(QString uuid)
{
    _dbi->unassignUser(_uuid, uuid);
    for (int i = _users.count() -1; i >= 0; i--)
    {
        RamUser *u = _users[i];
        if (u->uuid() == uuid)
        {
            _users.removeAt(i);
            emit userRemoved(uuid);
        }
    }
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

void RamStep::userDestroyed(QObject *o)
{
    RamUser *u = (RamUser*)o;
    removeUser(u);
}

int RamStep::order() const
{
    return _order;
}

void RamStep::setOrder(int order)
{
    _order = order;
    emit changed();
}

QString RamStep::projectUuid() const
{
    return _projectUuid;
}

void RamStep::setProjectUuid(const QString &projectUuid)
{
    _projectUuid = projectUuid;
}
