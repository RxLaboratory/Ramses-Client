#include "ramassetgroup.h"

RamAssetGroup::RamAssetGroup(QString shortName, QString name, bool tplt, QString uuid, QObject *parent) :
    RamObject(shortName, name, uuid, parent)
{
    _projectUuid = "";
    _template = tplt;
    if (_template) _dbi->createTemplateAssetGroup(_shortName, _name, _uuid);
}

RamAssetGroup::RamAssetGroup(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    _projectUuid = projectUuid;
    _template = false;
    _dbi->createAssetGroup(_shortName, _name, projectUuid, _uuid);
}

RamAssetGroup::~RamAssetGroup()
{
    if (_template) _dbi->removeTemplateAssetGroup(_uuid);
    else _dbi->removeAssetGroup(_uuid);
}

bool RamAssetGroup::isTemplate() const
{
    return _template;
}

RamAssetGroup *RamAssetGroup::createFromTemplate(QString projectUuid)
{
    // Create
    RamAssetGroup *assetGroup = new RamAssetGroup(_shortName, _name, projectUuid);
    return assetGroup;
}

QString RamAssetGroup::projectUuid() const
{
    return _projectUuid;
}

void RamAssetGroup::setProjectUuid(const QString &projectUuid)
{
    _projectUuid = projectUuid;
}

void RamAssetGroup::update()
{
    if (_template) _dbi->updateTemplateAssetGroup(_uuid, _shortName, _name);
    else _dbi->updateAssetGroup(_uuid, _shortName, _name);
}
