#include "ramassetgroup.h"
#include "ramproject.h"

RamAssetGroup::RamAssetGroup(QString shortName, QString name, QString uuid, QObject *parent) :
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(AssetGroup);
    m_project = nullptr;
    _template = true;
    if (_template) _dbi->createTemplateAssetGroup(_shortName, _name, _uuid);

    this->setObjectName( "RamAssetGroup (template) " + _shortName );
}

RamAssetGroup::RamAssetGroup(QString shortName, RamProject *project, QString name, QString uuid, QObject *parent):
    RamObjectList(shortName, name, uuid, parent)
{
    this->setObjectType(AssetGroup);
    m_project = project;
    _template = false;
    _dbi->createAssetGroup(_shortName, _name, m_project->uuid(), _uuid);

    this->setObjectName( "RamAssetGroup " + _shortName);
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

RamProject *RamAssetGroup::project() const
{
    return m_project;
}

void RamAssetGroup::append(RamAsset *asset)
{
    asset->setAssetGroup( this );
    asset->update();
    RamObjectList::append(asset);
}

void RamAssetGroup::createAsset(QString shortName, QString name)
{
    if (_template) return;
    RamAsset *asset = new RamAsset(shortName, m_project, this, name, "", this);
    append(asset);
}

void RamAssetGroup::update()
{
    if (!_dirty) return;
    RamObject::update();
    if (_template) _dbi->updateTemplateAssetGroup(_uuid, _shortName, _name);
    else _dbi->updateAssetGroup(_uuid, _shortName, _name);
}
