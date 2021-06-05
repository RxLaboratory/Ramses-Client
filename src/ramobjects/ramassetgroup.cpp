#include "ramassetgroup.h"

RamAssetGroup::RamAssetGroup(QString shortName, QString name, bool tplt, QString uuid, QObject *parent) :
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(AssetGroup);
    _projectUuid = "";
    _template = tplt;
    if (_template) _dbi->createTemplateAssetGroup(_shortName, _name, _uuid);
}

RamAssetGroup::RamAssetGroup(QString shortName, QString name, QString projectUuid, QString uuid, QObject *parent):
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(AssetGroup);
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
    if (projectUuid == _projectUuid) return;
    _projectUuid = projectUuid;
}

RamAsset *RamAssetGroup::asset(QString uuid) const
{
    return qobject_cast<RamAsset*>( this->fromUuid(uuid) );
}

void RamAssetGroup::append(RamAsset *asset)
{
    asset->setAssetGroupUuid( _uuid );
    asset->update();
    RamObjectList::append(asset);
}

void RamAssetGroup::createAsset(QString shortName, QString name)
{
    RamAsset *asset = new RamAsset(shortName, name, _uuid);
    append(asset);
}

void RamAssetGroup::update()
{
    if (!_dirty) return;
    RamObject::update();
    if (_template) _dbi->updateTemplateAssetGroup(_uuid, _shortName, _name);
    else _dbi->updateAssetGroup(_uuid, _shortName, _name);
}
