#include "ramassetgroup.h"

#include "assetgroupeditwidget.h"
#include "data-models/ramitemtable.h"

// STATIC //

QMap<QString, RamAssetGroup*> RamAssetGroup::m_existingObjects = QMap<QString, RamAssetGroup*>();

RamAssetGroup *RamAssetGroup::get(QString uuid)
{
    if (!checkUuid(uuid, AssetGroup)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamAssetGroup(uuid);
}

RamAssetGroup *RamAssetGroup::c(RamObject *o)
{
    return qobject_cast<RamAssetGroup*>(o);
}

RamAssetGroup *RamAssetGroup::createFromTemplate(RamTemplateAssetGroup *tempAG, RamProject *project)
{
    // Create
    RamAssetGroup *assetGroup = new RamAssetGroup(tempAG->shortName(), tempAG->name(), project);
    assetGroup->setColor(tempAG->color());
    project->assetGroups()->appendObject(assetGroup->uuid());
    return assetGroup;
}

// PUBLIC //

RamAssetGroup::RamAssetGroup(QString shortName, QString name, RamProject *project):
    RamTemplateAssetGroup(shortName, name, AssetGroup)
{
    construct();
    setProject(project);
}

RamAssetGroup::RamAssetGroup(QString uuid):
    RamTemplateAssetGroup(uuid, AssetGroup)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();

    setProject( RamProject::get(projUuid) );
}

int RamAssetGroup::assetCount() const
{
    return m_assets->rowCount();
}

RamProject *RamAssetGroup::project() const
{
    return m_project;
}

QString RamAssetGroup::details() const
{
    return "Contains " + QString::number(assetCount()) + " assets";
}

void RamAssetGroup::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new AssetGroupEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

QString RamAssetGroup::folderPath() const
{
    if (!m_project) return "";
    return m_project->path(RamObject::AssetsFolder) + "/" + name();
}

// PRIVATE //

void RamAssetGroup::construct()
{
    m_existingObjects[m_uuid] = this;
    m_objectType = AssetGroup;
    m_project = nullptr;
    m_assets = new RamObjectSortFilterProxyModel(this);
    m_assets->setSingleColumn(true);
    m_icon = ":/icons/asset-group";
}

void RamAssetGroup::setProject(RamProject *project)
{
    m_project = project;
    m_assets->setSourceModel( m_project->assets() );
    m_assets->setFilterUuid( m_uuid );
    this->setParent( m_project );
    insertData("project", project->uuid());
}

