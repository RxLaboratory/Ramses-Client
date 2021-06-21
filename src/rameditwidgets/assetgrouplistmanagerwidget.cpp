#include "assetgrouplistmanagerwidget.h"

AssetGroupListManagerWidget::AssetGroupListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new AssetGroupEditWidget(parent),
        "Asset groups",
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &AssetGroupListManagerWidget::changeProject);
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void AssetGroupListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamAssetGroup *assetGroup = new RamAssetGroup(
                "NEW",
                project,
                "New Asset Group");
    project->assetGroups()->append(assetGroup);
    editObject(assetGroup);
}

void AssetGroupListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    qDebug() << "Project set to " + project->shortName();
    this->setList( project->assetGroups() );
}
