#include "ramasset.h"
#include "ramassetgroup.h"

RamAsset::RamAsset(QString shortName, RamProject *project, RamAssetGroup *assetGroup, QString name, QString uuid) :
    RamItem(shortName, project, name, uuid, assetGroup)
{
    setObjectType(Asset);
    setProductionType(RamStep::AssetProduction);
    m_assetGroup = assetGroup;
    m_dbi->createAsset(m_shortName, m_name, m_assetGroup->uuid(), "", m_uuid);

    this->setObjectName( "RamAsset " + m_shortName);
}

RamAsset::~RamAsset()
{
    m_dbi->removeAsset(m_uuid);
}

RamAssetGroup *RamAsset::assetGroup() const
{
    return m_assetGroup;
}

void RamAsset::setAssetGroup(RamAssetGroup *assetGroup)
{
    if (m_assetGroup->is(assetGroup)) return;
    m_dirty = true;
    this->setParent(assetGroup);
    m_assetGroup = assetGroup;
    emit changed(this);
}

QStringList RamAsset::tags() const
{
    return _tags;
}

void RamAsset::setTags(QString tags)
{
    m_dirty = true;
    QStringList ts = tags.toLower().split(",");
    _tags.clear();
    foreach(QString t, ts)
    {
        _tags << t.trimmed().toLower();
    }
    emit changed(this);
}

void RamAsset::addTag(QString tag)
{
    if (_tags.contains(tag) ) return;
    m_dirty = true;
    _tags << tag.trimmed().toLower();
    emit changed(this);
}

void RamAsset::removeTag(QString tag)
{
    if (!_tags.contains(tag)) return;
    _tags.removeAll(tag.toLower());
    m_dirty = true;
    emit changed(this);
}

bool RamAsset::hasTag(QString tag)
{
    return _tags.contains(tag, Qt::CaseInsensitive);
}

void RamAsset::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updateAsset(m_uuid, m_shortName, m_name, m_assetGroup->uuid(), _tags.join(','));
}

RamAsset *RamAsset::asset( QString uuid )
{
    return qobject_cast<RamAsset*>( RamObject::obj(uuid) );
}

