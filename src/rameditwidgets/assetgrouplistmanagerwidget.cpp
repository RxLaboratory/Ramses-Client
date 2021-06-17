#include "assetgrouplistmanagerwidget.h"

AssetGroupListManagerWidget::AssetGroupListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new AssetGroupEditWidget(parent),
        "Asset groups",
        parent)
{
    this->setContainingType(RamObject::AssetGroup);
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &AssetGroupListManagerWidget::changeProject);
}

void AssetGroupListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->createAssetGroup();
}

void AssetGroupListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    qDebug() << "Project set to " + project->shortName();
    this->setList( qobject_cast<RamObjectList*>( project->assetGroups() ) );
}
