#include "assetlistmanagerwidget.h"

AssetListManagerWidget::AssetListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new AssetEditWidget(),
        "Assets",
        QIcon(":icons/asset"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *AssetListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    if (project->assetGroups()->count() == 0 ) return nullptr;
    RamAssetGroup *ag = RamAssetGroup::assetGroup( currentFilter() );
    if (!ag) ag = qobject_cast<RamAssetGroup*>( project->assetGroups()->at(0) );
    if(!ag) return nullptr;

    RamAsset *asset = new RamAsset(
                "NEW",
                ag,
                "New Asset"
                );

    project->assets()->append(asset);
    editObject(asset);
    return asset;
}

void AssetListManagerWidget::changeProject(RamProject *project)
{
    if (!project) return;
    this->setList( project->assets() );
    m_listEditWidget->setFilterList( project->assetGroups() );
}
