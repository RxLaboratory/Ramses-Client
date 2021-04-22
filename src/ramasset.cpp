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
    _assetGroupUuid = assetGroupUuid;
}

QStringList RamAsset::tags() const
{
    return _tags;
}

void RamAsset::setTags(QString tags)
{
    QStringList ts = tags.toLower().split(",");
    _tags.clear();
    foreach(QString t, ts)
    {
        _tags << t.trimmed();
    }
}

void RamAsset::addTag(QString tag)
{
    _tags << tag.trimmed().toLower();
}

void RamAsset::removeTag(QString tag)
{
    _tags.removeAll(tag.toLower());
}

bool RamAsset::hasTag(QString tag)
{
    return _tags.contains(tag, Qt::CaseInsensitive);
}

void RamAsset::update()
{
    _dbi->updateAsset(_uuid, _shortName, _name, _assetGroupUuid, _tags.join(','));
}

