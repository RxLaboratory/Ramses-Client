#include "assetgroupsmanagerwidget.h"

AssetGroupsManagerWidget::AssetGroupsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    assetGroupWidget = new AssetGroupEditWidget(this);
    this->setWidget(assetGroupWidget);
    assetGroupWidget->setEnabled(false);

    this->setRole(RamUser::ProjectAdmin);

    // Add menu
    assignMenu = new QMenu(this);
    actionCreateAssetGroup = new QAction("Create new asset group");
    assignMenu->addAction(actionCreateAssetGroup);
    assignMenu->addSeparator();
    addButton->setMenu(assignMenu);

    foreach(RamAssetGroup *assetGroup, Ramses::instance()->templateAssetGroups()) newTemplateAssetGroup(assetGroup);

    connect(actionCreateAssetGroup, &QAction::triggered, this, &AssetGroupsManagerWidget::createAssetGroup);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &AssetGroupsManagerWidget::changeProject);
    connect(Ramses::instance(), &Ramses::newTemplateAssetGroup, this, &AssetGroupsManagerWidget::newTemplateAssetGroup);
}

void AssetGroupsManagerWidget::currentDataChanged(QVariant data)
{
    assetGroupWidget->setEnabled(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamAssetGroup *assetGroup = project->assetGroup( data.toString() );
    if (!assetGroup) return;

    assetGroupWidget->setAssetGroup(assetGroup);
    assetGroupWidget->setEnabled(true);
}

void AssetGroupsManagerWidget::removeItem(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->removeAssetGroup(data.toString());
}

void AssetGroupsManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    list->blockSignals(true);
    list->clear();
    assetGroupWidget->setAssetGroup(nullptr);
    list->blockSignals(false);

    if (!project) return;

    //add steps
    foreach(RamAssetGroup *assetGroup, project->assetGroups()) newAssetGroup(assetGroup);
    _projectConnections << connect(project, &RamProject::newAssetGroup, this, &AssetGroupsManagerWidget::newAssetGroup);
    _projectConnections << connect(project, SIGNAL(assetGroupRemoved(QString)), this, SLOT(assetGroupRemoved(QString)));

    this->setEnabled(true);
}

void AssetGroupsManagerWidget::assignAssetGroup()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    QAction *assetGroupAction = (QAction*)sender();
    RamAssetGroup *templateAssetGroup = Ramses::instance()->templateAssetGroup(assetGroupAction->data().toString());
    if (!templateAssetGroup) return;
    project->assignAssetGroup(templateAssetGroup);
}

void AssetGroupsManagerWidget::createAssetGroup()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    project->createAssetGroup();
}

void AssetGroupsManagerWidget::newAssetGroup(RamAssetGroup *assetGroup)
{
    if (!assetGroup) return;

    if (assetGroup->uuid() != "")
    {
        QListWidgetItem *assetGroupItem = new QListWidgetItem(assetGroup->name());
        assetGroupItem->setData(Qt::UserRole, assetGroup->uuid());
        this->addItem(assetGroupItem);
        connect(assetGroup, SIGNAL(removed(RamObject*)), this, SLOT(assetGroupRemoved(RamObject*)));
        connect(assetGroup, &RamAssetGroup::changed, this, &AssetGroupsManagerWidget::assetGroupChanged);
    }
}

void AssetGroupsManagerWidget::assetGroupRemoved(QString uuid)
{
    removeData(uuid);
}

void AssetGroupsManagerWidget::assetGroupRemoved(RamObject *o)
{
    removeData(o->uuid());
}

void AssetGroupsManagerWidget::assetGroupChanged()
{
    RamAssetGroup *ag = (RamAssetGroup*)QObject::sender();
    updateItem(ag->uuid(), ag->name());
}

void AssetGroupsManagerWidget::newTemplateAssetGroup(RamAssetGroup *assetGroup)
{
    if (!assetGroup) return;
    if (assetGroup->uuid() == "") return;
    QAction *agAction = new QAction(assetGroup->name());
    agAction->setData(assetGroup->uuid());
    assignMenu->addAction(agAction);
    connect(agAction, &QAction::triggered, this, &AssetGroupsManagerWidget::assignAssetGroup);
    connect(assetGroup, &RamAssetGroup::removed, this, &AssetGroupsManagerWidget::templateAssetGroupRemoved);
    connect(assetGroup, &RamAssetGroup::changed, this, &AssetGroupsManagerWidget::templateAssetGroupChanged);
}

void AssetGroupsManagerWidget::templateAssetGroupRemoved(RamObject *o)
{
    QList<QAction *> actions = assignMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == o->uuid())
        {
            assignMenu->removeAction(actions[i]);
            actions[i]->deleteLater();
        }
    }
}

void AssetGroupsManagerWidget::templateAssetGroupChanged()
{
    RamAssetGroup *ag = (RamAssetGroup*)sender();
    QList<QAction *> actions = assignMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == ag->uuid()) actions[i]->setText(ag->name());
    }
}
