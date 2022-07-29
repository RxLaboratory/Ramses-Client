#include "ramassetgroup.h"
#include "ramproject.h"
#include "ramses.h"

#include "assetgroupeditwidget.h"
#include "templateassetgroupeditwidget.h"

// STATIC //

RamAssetGroup *RamAssetGroup::assetGroup(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::obj(uuid);
    if (!obj && constructNew) return new RamAssetGroup( uuid );
    return qobject_cast<RamAssetGroup*>( obj );
}

// PUBLIC //

RamAssetGroup::RamAssetGroup(QString shortName, QString name) :
    RamObject(shortName, name, AssetGroup)
{
    construct();
    insertData("template", true);
}

RamAssetGroup::RamAssetGroup(QString shortName, QString name, RamProject *project):
    RamObject(shortName, name, AssetGroup, project)
{
    construct();
    insertData("template", false);
    setProject(project);
}

bool RamAssetGroup::isTemplate() const
{
    return data().value("template").toBool(true);
}

RamAssetGroup *RamAssetGroup::createFromTemplate(RamProject *project)
{
    // Create
    RamAssetGroup *assetGroup = new RamAssetGroup(shortName(), name(), project);
    return assetGroup;
}

int RamAssetGroup::assetCount() const
{
    return m_assets->rowCount();
}

RamProject *RamAssetGroup::project() const
{
    return m_project;
}

void RamAssetGroup::edit(bool show)
{
    if (!ui_editWidget)
    {
        if (this->isTemplate())
            setEditWidget(new TemplateAssetGroupEditWidget(this));
        else
            setEditWidget(new AssetGroupEditWidget(this));
    }

    if (show) showEdit();
}

// PROTECTED //

RamAssetGroup::RamAssetGroup(QString uuid):
    RamObject(uuid, AssetGroup)
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
    if (isTemplate()) return "";
    return m_project->path(RamObject::AssetsFolder) + "/" + name();
}

// PRIVATE //

void RamAssetGroup::construct()
{
    m_icon = ":/icons/asset-group";
    m_editRole = Admin;
    m_project = nullptr;
    m_assets = new RamObjectFilterModel(this);
}

void RamAssetGroup::setProject(RamProject *project)
{
    m_project = project;
    m_assets->setSourceModel( m_project->assets() );
    m_assets->setFilterUuid( m_uuid );
    this->setParent( m_project );
}

