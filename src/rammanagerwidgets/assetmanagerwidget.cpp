#include "assetmanagerwidget.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramses.h"
#include "duwidgets/duicon.h"

AssetManagerWidget::AssetManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Assets",
        DuIcon(":icons/asset"),
        parent)
{
    changeProject(Ramses::i()->project());
    connect(Ramses::i(), &Ramses::ready, this, [this]() { changeProject(Ramses::i()->project()); });
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamAbstractItem *AssetManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;
    if (project->assetGroups()->rowCount() == 0 ) return nullptr;

    RamAssetGroup *ag = RamAssetGroup::c( currentFilter() );
    if (!ag) ag = RamAssetGroup::c( project->assetGroups()->get(0) );
    if (!ag) return nullptr;

    RamAsset *asset = new RamAsset(
                "NEW",
                "New Asset",
                ag
                );

    //project->assets()->appendObject(asset->uuid());
    asset->edit();
    return asset;
}

void AssetManagerWidget::changeProject(RamProject *project)
{
    if (!project) return;
    this->setObjectModel( project->assets() );
    ui_listWidget->setFilterList( project->assetGroups(), "Assets" );
}
