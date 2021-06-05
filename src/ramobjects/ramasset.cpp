#include "ramasset.h"

RamAsset::RamAsset(QString shortName, QString name, QString assetGroupUuid, QString uuid, QObject *parent) :
    RamItem(shortName, name, uuid, parent)
{
    setObjectType(Asset);
    _assetGroupUuid = assetGroupUuid;
    _dbi->createAsset(_shortName, _name, _assetGroupUuid, "", _uuid);
}

RamAsset::~RamAsset()
{
    _dbi->removeAsset(_uuid);
}

QString RamAsset::assetGroupUuid() const
{
    return _assetGroupUuid;
}

void RamAsset::setAssetGroupUuid(const QString &assetGroupUuid)
{
    if (assetGroupUuid == _assetGroupUuid) return;
    _dirty = true;
    _assetGroupUuid = assetGroupUuid;
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
    _dbi->updateAsset(_uuid, _shortName, _name, _assetGroupUuid, _tags.join(','));
}

