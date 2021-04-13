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

QList<RamAsset *> RamAssetGroup::assets() const
{
    return _assets;
}

RamAsset *RamAssetGroup::asset(QString uuid) const
{
    foreach (RamAsset *a, _assets)
    {
        if (a->uuid() == uuid) return a;
    }
    return nullptr;
}

void RamAssetGroup::addAsset(RamAsset *asset)
{
    if (_assets.contains(asset)) return;
    _assets << asset;
    asset->setAssetGroupUuid( _uuid );
    asset->update();
    connect(asset, SIGNAL(removed(RamObject*)), this, SLOT(assetRemoved(RamObject*)));
    emit newAsset(asset);
}

void RamAssetGroup::createAsset(QString shortName, QString name)
{
    RamAsset *asset = new RamAsset(shortName, name, _uuid);
    addAsset(asset);
}

void RamAssetGroup::removeAsset(QString uuid)
{
    for (int i = _assets.count() -1; i >= 0; i--)
    {
        RamAsset *a = _assets[i];
        if (a->uuid() == uuid)
        {
            _assets.removeAt(i);
            emit assetRemoved(uuid);
        }
    }
}

void RamAssetGroup::removeAsset(RamObject *asset)
{
    removeAsset(asset->uuid());
}

bool assetSorter(RamAsset *a, RamAsset *b)
{
    return a->shortName() > b->shortName();
}

void RamAssetGroup::sortAssets()
{
    std::sort(_assets.begin(), _assets.end(), assetSorter);
}

void RamAssetGroup::update()
{
    if (_template) _dbi->updateTemplateAssetGroup(_uuid, _shortName, _name);
    else _dbi->updateAssetGroup(_uuid, _shortName, _name);
}

void RamAssetGroup::assetRemoved(RamObject *o)
{
    removeAsset(o);
}

