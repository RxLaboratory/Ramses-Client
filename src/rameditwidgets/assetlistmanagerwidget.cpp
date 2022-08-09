#include "assetlistmanagerwidget.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramses.h"

AssetListManagerWidget::AssetListManagerWidget(QWidget *parent):
    ObjectListManagerWidget<RamItem*, RamAssetGroup*>(
        "Assets",
        QIcon(":icons/asset"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamItem*, RamAssetGroup*>::RemoveObjects);
}

RamItem *AssetListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    if (project->assetGroups()->rowCount() == 0 ) return nullptr;
    RamAssetGroup *ag = RamAssetGroup::getObject( currentFilterUuid() );
    if (!ag) ag = project->assetGroups()->at(0);
    if (!ag) return nullptr;

    RamAsset *asset = new RamAsset(
                "NEW",
                "New Asset",
                ag
                );

    project->assets()->append(asset);
    asset->edit();
    return asset;
}

void AssetListManagerWidget::changeProject(RamProject *project)
{
    if (!project) return;
    this->setList( project->assets() );
    m_listEditWidget->setFilterList( project->assetGroups() );
}
