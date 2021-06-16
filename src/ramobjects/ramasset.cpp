#include "ramasset.h"
#include "ramassetgroup.h"

RamAsset::RamAsset(QString shortName, RamProject *project, RamAssetGroup *assetGroup, QString name, QString uuid, QObject *parent) :
    RamItem(shortName, project, name, uuid, parent)
{
    setObjectType(Asset);
    setProductionType(RamStep::AssetProduction);
    m_assetGroup = assetGroup;
    _dbi->createAsset(_shortName, _name, m_assetGroup->uuid(), "", _uuid);

    this->setObjectName( "RamAsset " + _shortName);
}

RamAsset::~RamAsset()
{
    _dbi->removeAsset(_uuid);
}

RamAssetGroup *RamAsset::assetGroup() const
{
    return m_assetGroup;
}

void RamAsset::setAssetGroup(RamAssetGroup *assetGroup)
{
    if (m_assetGroup->is(assetGroup)) return;
    _dirty = true;
    m_assetGroup = assetGroup;
}

QStringList RamAsset::tags() const
{
    return _tags;
}

void RamAsset::setTags(QString tags)
{
    _dirty = true;
    QStringList ts = tags.toLower().split(",");
    _tags.clear();
    foreach(QString t, ts)
    {
        _tags << t.trimmed();
    }
}

void RamAsset::addTag(QString tag)
{
    if (_tags.contains(tag) ) return;
    _dirty = true;
    _tags << tag.trimmed().toLower();
}

void RamAsset::removeTag(QString tag)
{
    if (!_tags.contains(tag)) return;
    _tags.removeAll(tag.toLower());
    _dirty = true;
}

bool RamAsset::hasTag(QString tag)
{
    return _tags.contains(tag, Qt::CaseInsensitive);
}

void RamAsset::update()
{
    if (!_dirty) return;
    RamObject::update();
    _dbi->updateAsset(_uuid, _shortName, _name, m_assetGroup->uuid(), _tags.join(','));
}

RamAsset *RamAsset::asset( QString uuid )
{
    return qobject_cast<RamAsset*>( RamObject::obj(uuid) );
}

