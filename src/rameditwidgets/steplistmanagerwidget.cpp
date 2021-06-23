#include "steplistmanagerwidget.h"

StepListManagerWidget::StepListManagerWidget(QWidget *parent):
ObjectListManagerWidget(
    new StepEditWidget(),
    "Steps",
    parent )
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);

    // Create from template actions
    ui_createMenu = new QMenu(this);
    QAction *createAction = new QAction("Create new step");
    ui_createMenu->addAction(createAction);
    ui_createMenu->addSeparator();
    QToolButton *addButton = m_listEditWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    // Add templates
    RamObjectList *templateSteps = Ramses::instance()->templateSteps();
    for (int i = 0; i < templateSteps->count(); i++) newTemplate( templateSteps->at(i) );

    connect(createAction, SIGNAL(triggered()), this, SLOT(createObject()));
    connect(templateSteps, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(templateStepInserted(QModelIndex,int,int)));
    connect(templateSteps, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(templateStepRemoved(QModelIndex,int,int)));

}

void StepListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStep *step = new RamStep(
                "NEW",
                "New Step",
                project
                );
    project->steps()->append(step);
    editObject(step);
}

void StepListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->setList(nullptr);
    if (!project) return;
    this->setList( project->steps() );
}

void StepListManagerWidget::templateStepInserted(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *o = Ramses::instance()->templateSteps()->at(i);
        newTemplate(o);
    }
}

void StepListManagerWidget::newTemplate(RamObject *obj)
{
    QAction *action = new QAction( obj->name() );
    quintptr iptr = reinterpret_cast<quintptr>( obj );
    action->setData( iptr );
    ui_createMenu->addAction(action);

    connect(action, SIGNAL(triggered()), this, SLOT(actionCreate()));
    connect(obj, &RamObject::changed, this, &StepListManagerWidget::templateStepChanged);

}

void StepListManagerWidget::templateStepRemoved(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    QList<QAction *> actions = ui_createMenu->actions();

    for (int i = first; i <= last; i++)
    {
        RamObject *removedObj = Ramses::instance()->templateSteps()->at(i);
        for (int j = actions.count() -1; j >= 0; j--)
        {
            quintptr iptr = actions.at(j)->data().toULongLong();
            RamObject *obj = reinterpret_cast<RamObject*>( iptr );

            if (removedObj->is(obj)) actions.at(j)->deleteLater();
            break;
        }
    }
}

void StepListManagerWidget::templateStepChanged()
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

void StepListManagerWidget::actionCreate()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    QAction *stepAction = (QAction*)sender();
    quintptr iptr = stepAction->data().toULongLong();
    RamStep *templateStep = reinterpret_cast<RamStep*>( iptr );
    if (!templateStep) return;
    RamStep *step = templateStep->createFromTemplate(project);
    project->steps()->append(step);
    editObject(step);
}
