#include "assetgrouplistmanagerwidget.h"

AssetGroupListManagerWidget::AssetGroupListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new AssetGroupEditWidget(parent),
        "Asset groups",
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
    this->setList( project->assetGroups() );
}

void AssetGroupListManagerWidget::createFromTemplate(RamObject *obj)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamAssetGroup *templateAG = qobject_cast<RamAssetGroup*>( obj );
    if (!templateAG) return;
    RamAssetGroup *ag = templateAG->createFromTemplate(project);
    project->assetGroups()->append(ag);
    editObject(ag);
}
