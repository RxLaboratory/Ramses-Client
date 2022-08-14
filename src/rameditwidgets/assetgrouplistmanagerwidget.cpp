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

    connect(ui_createMenu, &RamObjectListMenu::createTriggered, this, &AssetGroupListManagerWidget::createObject);
    connect(ui_createMenu, &RamObjectListMenu::assigned, this, &AssetGroupListManagerWidget::createFromTemplate);
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

void AssetGroupListManagerWidget::createFromTemplate(RamObject *templateAGObj)
{
    RamTemplateAssetGroup *templateAG = RamTemplateAssetGroup::c(templateAGObj);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (!templateAG) return;
    RamAssetGroup *ag = RamAssetGroup::createFromTemplate( templateAG, project);
    ag->edit();
}
