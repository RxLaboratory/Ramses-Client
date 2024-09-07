#include "assetgroupmanagerwidget.h"

#include "duwidgets/duicon.h"
#include "ramassetgroup.h"
#include "ramses.h"

AssetGroupManagerWidget::AssetGroupManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Asset groups",
        DuIcon(":icons/asset-group"),
        parent)
{
    changeProject(Ramses::i()->project());
    connect(Ramses::i(), &Ramses::ready, this, [this]() { changeProject(Ramses::i()->project()); });
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);

    // Create from template actions
    ui_createMenu = new RamObjectMenu(false, this);
    ui_createMenu->addCreateButton();
    QToolButton *addButton = ui_listWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    ui_createMenu->setObjectModel(Ramses::i()->templateAssetGroups());

    connect(ui_createMenu, &RamObjectMenu::createTriggered, this, &AssetGroupManagerWidget::createObject);
    connect(ui_createMenu, &RamObjectMenu::assigned, this, &AssetGroupManagerWidget::createFromTemplate);
}

RamAssetGroup *AssetGroupManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;
    RamAssetGroup *assetGroup = new RamAssetGroup(
                "NEW",
                "New Asset Group",
                project
                );
    //project->assetGroups()->appendObject(assetGroup->uuid());
    assetGroup->edit();
    return assetGroup;
}

void AssetGroupManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->assetGroups() );
}

void AssetGroupManagerWidget::createFromTemplate(RamObject *templateAGObj)
{
    RamTemplateAssetGroup *templateAG = RamTemplateAssetGroup::c(templateAGObj);

    RamProject *project = Ramses::i()->project();
    if (!project) return;
    if (!templateAG) return;
    RamAssetGroup *ag = RamAssetGroup::createFromTemplate( templateAG, project);
    ag->edit();
}
