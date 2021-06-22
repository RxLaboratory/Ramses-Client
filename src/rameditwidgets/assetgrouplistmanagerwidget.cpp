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

    // Create from template actions
    ui_createMenu = new QMenu(this);
    QAction *createAction = new QAction("Create new asset group");
    ui_createMenu->addAction(createAction);
    ui_createMenu->addSeparator();
    QToolButton *addButton = m_listEditWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    // Add templates
    RamObjectList *templateAGs = Ramses::instance()->templateAssetGroups();
    for (int i = 0; i < templateAGs->count(); i++) newTemplate( templateAGs->at(i) );

    connect(createAction, SIGNAL(triggered()), this, SLOT(createObject()));
    connect(templateAGs, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(templateInserted(QModelIndex,int,int)));
    connect(templateAGs, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(templateRemoved(QModelIndex,int,int)));

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

void AssetGroupListManagerWidget::templateInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *o = Ramses::instance()->templateAssetGroups()->at(i);
        newTemplate(o);
    }
}

void AssetGroupListManagerWidget::newTemplate(RamObject *obj)
{
    QAction *action = new QAction( obj->name() );
    quintptr iptr = reinterpret_cast<quintptr>( obj );
    action->setData( iptr );
    ui_createMenu->addAction(action);

    connect(action, SIGNAL(triggered()), this, SLOT(actionCreate()));
    connect(obj, &RamObject::changed, this, &AssetGroupListManagerWidget::templateChanged);

}

void AssetGroupListManagerWidget::templateRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QList<QAction *> actions = ui_createMenu->actions();

    for (int i = first; i <= last; i++)
    {
        RamObject *removedObj = Ramses::instance()->templateAssetGroups()->at(i);
        for (int j = actions.count() -1; j >= 0; j--)
        {
            quintptr iptr = actions.at(j)->data().toULongLong();
            RamObject *obj = reinterpret_cast<RamObject*>( iptr );

            if (removedObj->is(obj)) actions.at(j)->deleteLater();
            break;
        }
    }
}

void AssetGroupListManagerWidget::templateChanged()
{
    RamObject *changedObj = qobject_cast<RamObject*>( sender() );
    QList<QAction *> actions = ui_createMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        quintptr iptr = actions.at(i)->data().toULongLong();
        RamObject *obj = reinterpret_cast<RamObject*>( iptr );

        if (changedObj->is(obj)) actions.at(i)->setText(changedObj->name());
    }
}

void AssetGroupListManagerWidget::actionCreate()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    QAction *agAction = (QAction*)sender();
    quintptr iptr = agAction->data().toULongLong();
    RamAssetGroup *templateAG = reinterpret_cast<RamAssetGroup*>( iptr );
    if (!templateAG) return;
    RamAssetGroup *ag = templateAG->createFromTemplate(project);
    project->assetGroups()->append(ag);
    editObject(ag);
}
