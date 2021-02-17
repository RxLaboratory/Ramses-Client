#include "ramstep.h"

RamStep::RamStep(QString shortName, QString name, bool tplt, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _template = tplt;
    _type = AssetProduction;
    if (_template) _dbi->createTemplateStep(_name, _shortName, _uuid);
}

RamStep::~RamStep()
{
    _dbi->removeTemplateStep(_uuid);
}

bool RamStep::isTemplate() const
{
    return _template;
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

void RamStep::update()
{
    QString type = "asset";
    if (_type == PostProduction) type = "post";
    else if (_type == PreProduction) type = "pre";
    else if (_type == ShotProduction) type = "shot";
    _dbi->updateTemplateStep(_uuid, _shortName, _name, type);
}
