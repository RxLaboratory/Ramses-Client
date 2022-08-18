#include "ramasset.h"

#include "asseteditwidget.h"
#include "ramproject.h"

// STATIC //

RamAsset *RamAsset::get(QString uuid)
{
    return c( RamObject::get(uuid, Asset) );
}

RamAsset *RamAsset::c(RamObject *o)
{
    return qobject_cast<RamAsset*>(o);
}

// PUBLIC //

RamAsset::RamAsset(QString shortName, QString name, RamAssetGroup *ag) :
    RamItem(shortName, name, Asset, ag->project())
{
    Q_ASSERT_X(ag, "RamAsset(shortname, name, assetgroup)", "AssetGroup can't be null!");
    construct();
    setAssetGroup(ag);
}

RamAsset::RamAsset(QString uuid):
    RamItem(uuid, Asset)
{
    construct();
}

RamAssetGroup *RamAsset::assetGroup() const
{
    return RamAssetGroup::get( getData("assetGroup").toString() );
}

void RamAsset::setAssetGroup(RamAssetGroup *ag)
{
    if(!ag) return;
    insertData("assetGroup", ag->uuid());
}

QStringList RamAsset::tags() const
{
    QJsonArray arr = getData("tags").toArray();
    QStringList ts;
    for (int i = 0; i < arr.count(); i++)
    {
        ts << arr.at(i).toString();
    }
    return ts;
}

void RamAsset::setTags(QString tags)
{
    QStringList ts = tags.toLower().split(",");
    QJsonArray arr;
    for(int i = 0; i < ts.count(); i++)
    {
        arr.append( ts[i].trimmed().toLower() );
    }
    insertData("tags", arr);
}

void RamAsset::addTag(QString tag)
{
    QJsonArray arr = getData("tags").toArray();
    arr.append(tag.trimmed().toLower());
    insertData("tags", arr);
}

void RamAsset::removeTag(QString tag)
{
    QJsonArray arr = getData("tags").toArray();
    if (!arr.contains(tag)) return;
    for (int i = 0; i < arr.count(); i++)
    {
        if (arr.at(i).toString() == tag)
        {
            arr.removeAt(i);
            break;
        }
    }
    insertData("tags", arr);
}

bool RamAsset::hasTag(QString tag)
{
    QJsonArray arr = getData("tags").toArray();
    return arr.contains(tag);
}

QString RamAsset::filterUuid() const
{
    return getData("assetGroup").toString();
}

QString RamAsset::details() const
{
    return assetGroup()->name() +
            "\n" +
            tags().join(", ");
}

void RamAsset::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new AssetEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

QString RamAsset::folderPath() const
{
    return  assetGroup()->path() + "/" + m_project->shortName() + "_A_" + shortName();
}

// PRIVATE //

void RamAsset::construct()
{
    m_icon = ":/icons/asset";
    m_editRole = ProjectAdmin;
    m_productionType = RamStep::AssetProduction;
}

