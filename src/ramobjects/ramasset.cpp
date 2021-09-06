#include "ramasset.h"
#include "ramassetgroup.h"
#include "asseteditwidget.h"

#include "ramproject.h"

RamAsset::RamAsset(QString shortName, RamAssetGroup *assetGroup, QString name, QString uuid) :
    RamItem(shortName, assetGroup->project(), name, uuid)
{
    setObjectType(Asset);
    setProductionType(RamStep::AssetProduction);
    m_assetGroup = assetGroup;
    m_filterUuid = assetGroup->uuid();
    m_assetGroupConnection = connect(assetGroup, SIGNAL(removed(RamObject*)), this, SLOT(remove()));
    m_dbi->createAsset(m_shortName, m_name, m_assetGroup->uuid(), "", m_uuid);

    this->setObjectName( "RamAsset " + m_shortName);
}

RamAsset::~RamAsset()
{

}

RamAssetGroup *RamAsset::assetGroup() const
{
    return m_assetGroup;
}

void RamAsset::setAssetGroup(RamAssetGroup *assetGroup)
{
    if(!assetGroup) return;
    if (m_assetGroup->is(assetGroup)) return;
    m_dirty = true;

    disconnect(m_assetGroupConnection);

    this->setParent(assetGroup);
    m_assetGroup = assetGroup;
    m_filterUuid = assetGroup->uuid();

    m_assetGroupConnection = connect(assetGroup, SIGNAL(removed(RamObject*)), this, SLOT(remove()));

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
    m_dbi->updateAsset(m_uuid, m_shortName, m_name, m_assetGroup->uuid(), _tags.join(','), m_comment);
}

RamAsset *RamAsset::asset( QString uuid )
{
    return qobject_cast<RamAsset*>( RamObject::obj(uuid) );
}

void RamAsset::edit(bool show)
{
    if (!m_editReady)
    {
        AssetEditWidget *w = new AssetEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    showEdit(show);
}

void RamAsset::removeFromDB()
{
    m_dbi->removeAsset(m_uuid);
}

QString RamAsset::folderPath() const
{
    RamProject *p = m_assetGroup->project();
    return  m_assetGroup->path() + "/" + p->shortName() + "_A_" + m_shortName;
}

