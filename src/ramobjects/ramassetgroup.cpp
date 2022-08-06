#include "ramassetgroup.h"

#include "assetgroupeditwidget.h"

// STATIC //

RamAssetGroup *RamAssetGroup::getObject(QString uuid, bool constructNew)
{
    RamTemplateAssetGroup *obj = RamTemplateAssetGroup::getObject(uuid);
    if (!obj && constructNew) return new RamAssetGroup( uuid );
    return qobject_cast<RamAssetGroup*>( obj );
}

RamAssetGroup *RamAssetGroup::createFromTemplate(RamTemplateAssetGroup *tempAG, RamProject *project)
{
    // Create
    RamAssetGroup *assetGroup = new RamAssetGroup(tempAG->shortName(), tempAG->name(), project);
    project->assetGroups()->append(assetGroup);
    return assetGroup;
}

// PUBLIC //

RamAssetGroup::RamAssetGroup(QString shortName, QString name, RamProject *project):
    RamTemplateAssetGroup(shortName, name)
{
    construct();
    setProject(project);
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

RamAssetGroup::RamAssetGroup(QString uuid):
    RamTemplateAssetGroup(uuid)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();

    if (!d.value("template").toBool(true) && projUuid != "")
    {
        setProject( RamProject::getObject(projUuid, true) );
    }
}

QString RamAssetGroup::folderPath() const
{
    return m_project->path(RamObject::AssetsFolder) + "/" + name();
}

// PRIVATE //

void RamAssetGroup::construct()
{
    m_objectType = AssetGroup;
    m_project = nullptr;
    m_assets = new RamObjectFilterModel<RamAsset*>(this);
}

void RamAssetGroup::setProject(RamProject *project)
{
    m_project = project;
    m_assets->setList( m_project->assets() );
    m_assets->setFilterUuid( m_uuid );
    this->setParent( m_project );
}

