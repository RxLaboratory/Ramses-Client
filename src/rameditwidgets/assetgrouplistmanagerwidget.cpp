#include "assetgrouplistmanagerwidget.h"

#include "ramassetgroup.h"
#include "ramses.h"

AssetGroupListManagerWidget::AssetGroupListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        "Asset groups",
        QIcon(":icons/asset-group"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);

    // Create from template actions
    ui_createMenu = new RamObjectListMenu(false, this);
    ui_createMenu->addCreateButton();
    QToolButton *addButton = m_listEditWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    ui_createMenu->setList(Ramses::instance()->templateAssetGroups());

    connect(ui_createMenu, SIGNAL(create()), this, SLOT(createObject()));
    connect(ui_createMenu, SIGNAL(assign(RamObject*)), this, SLOT(createFromTemplate(RamObject*)));
}

RamAssetGroup *AssetGroupListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    RamAssetGroup *assetGroup = new RamAssetGroup(
                "NEW",
                "New Asset Group",
                project
                );
    project->assetGroups()->append(assetGroup);
    assetGroup->edit();
    return assetGroup;
}

void AssetGroupListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( project->assetGroups() );
}

void AssetGroupListManagerWidget::createFromTemplate(RamTemplateAssetGroup *templateAG)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (!templateAG) return;
    RamAssetGroup *ag = RamAssetGroup::createFromTemplate( templateAG, project);
    ag->edit();
}
