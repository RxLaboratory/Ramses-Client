#include "assetlistmanagerwidget.h"

#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramses.h"
#include "data-models/ramitemtable.h"

AssetListManagerWidget::AssetListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        "Assets",
        QIcon(":icons/asset"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortable(true);
}

RamAbstractItem *AssetListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    if (project->assetGroups()->rowCount() == 0 ) return nullptr;

    RamAssetGroup *ag = RamAssetGroup::c( currentFilter() );
    qDebug() << ag;
    if (!ag) ag = RamAssetGroup::c( project->assetGroups()->at(0) );
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
