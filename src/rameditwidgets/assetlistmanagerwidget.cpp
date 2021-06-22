#include "assetlistmanagerwidget.h"

AssetListManagerWidget::AssetListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new AssetEditWidget(),
        "Assets",
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &AssetListManagerWidget::changeProject);
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void AssetListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (project->assetGroups()->count() == 0 ) return;
    RamAssetGroup *ag = RamAssetGroup::assetGroup( currentFilter() );
    if (!ag) ag = qobject_cast<RamAssetGroup*>( project->assetGroups()->at(0) );
    if(!ag) return;

    RamAsset *asset = new RamAsset(
                "NEW",
                ag,
                "New Asset"
                );

    project->assets()->append(asset);
    editObject(asset);
}

void AssetListManagerWidget::changeProject(RamProject *project)
{
    if (!project) return;
    this->setList( project->assets() );
}
