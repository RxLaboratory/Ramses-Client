#include "stepsmanagerwidget.h"

StepsManagerWidget::StepsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    stepWidget = new StepEditWidget(this);
    this->setWidget(stepWidget);
    stepWidget->setEnabled(false);

    this->setRole(RamUser::ProjectAdmin);

    list->setDragEnabled(true);

    // Add menu
    assignMenu = new QMenu(this);
    actionCreateStep = new QAction("Create new step");
    assignMenu->addAction(actionCreateStep);
    assignMenu->addSeparator();
    addButton->setMenu(assignMenu);

    // Add order buttons
    upButton = new QToolButton(this);
    upButton->setIcon(QIcon(":/icons/move-up"));
    downButton = new QToolButton(this);
    downButton->setIcon(QIcon(":/icons/move-down"));
    buttonsLayout->insertWidget(2, downButton);
    buttonsLayout->insertWidget(2, upButton);

    for (int i = 0; i < Ramses::instance()->templateSteps()->count(); i++) newTemplateStep(Ramses::instance()->templateSteps()->at(i));

    connect(actionCreateStep, &QAction::triggered, this, &StepsManagerWidget::createStep);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &StepsManagerWidget::changeProject);
    connect(Ramses::instance()->templateSteps(), &RamObjectList::objectAdded, this, &StepsManagerWidget::newTemplateStep);
    connect(Ramses::instance()->templateSteps(), &RamObjectList::objectRemoved, this, &StepsManagerWidget::templateStepRemoved);
    connect(upButton, &QToolButton::clicked, this, &StepsManagerWidget::moveStepUp);
    connect(downButton, &QToolButton::clicked, this, &StepsManagerWidget::moveStepDown);
    connect(list, &DuQFListWidget::itemDropped, this, &StepsManagerWidget::updateStepsOrder);
}

void StepsManagerWidget::currentDataChanged(QVariant data)
{
    stepWidget->setEnabled(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStep *step = project->step( data.toString() );
    if (!step) return;

    stepWidget->setStep(step);
    stepWidget->setEnabled(true);
}

void StepsManagerWidget::removeItem(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->removeStep(data.toString());
}

void StepsManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    list->blockSignals(true);
    list->clear();
    stepWidget->setStep(nullptr);
    list->blockSignals(false);

    if (!project) return;

    //add steps
    foreach(RamStep *step, project->steps()) newStep(step);
    _projectConnections << connect(project, &RamProject::newStep, this, &StepsManagerWidget::newStep);
    _projectConnections << connect(project, SIGNAL(stepRemoved(RamStep*)), this, SLOT(stepRemoved(RamStep*)));

    this->setEnabled(true);
}

void StepsManagerWidget::assignStep()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    QAction *stepAction = (QAction*)sender();
    RamStep *templateStep = Ramses::instance()->templateStep(stepAction->data().toString());
    if (!templateStep) return;
    project->assignStep(templateStep);
}

void StepsManagerWidget::createStep()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    project->createStep();
}

void StepsManagerWidget::newStep(RamStep *step)
{
    if (!step) return;

    if (step->uuid() != "")
    {
        QListWidgetItem *stepItem = new QListWidgetItem(step->name());
        stepItem->setData(Qt::UserRole, step->uuid());
        this->insertItem(step->order(), stepItem);
        connect(step, SIGNAL(removed(RamObject*)), this, SLOT(stepRemoved(RamObject*)));
        connect(step, &RamStep::changed, this, &StepsManagerWidget::stepChanged);
    }
}

void StepsManagerWidget::stepRemoved(QString uuid)
{
    removeData(uuid);
}

void StepsManagerWidget::stepRemoved(RamObject *step)
{
    removeData(step->uuid());
}

void StepsManagerWidget::stepRemoved(RamStep *step)
{
    removeData(step->uuid());
}

void StepsManagerWidget::stepChanged()
{
    RamStep *step = (RamStep*)QObject::sender();
    updateItem(step->uuid(), step->name());
}

void StepsManagerWidget::newTemplateStep(RamObject *step)
{
    if (!step) return;
    if (step->uuid() == "") return;
    QAction *stepAction = new QAction(step->name());
    stepAction->setData(step->uuid());
    assignMenu->addAction(stepAction);
    connect(stepAction, &QAction::triggered, this, &StepsManagerWidget::assignStep);
    connect(step, &RamStep::changed, this, &StepsManagerWidget::templateStepChanged);
}

void StepsManagerWidget::templateStepRemoved(RamObject *o)
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

void StepsManagerWidget::templateStepChanged()
{
    RamStep *s = (RamStep*)sender();
    QList<QAction *> actions = assignMenu->actions();
    for (int i = actions.count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == s->uuid()) actions[i]->setText(s->name());
    }
}

void StepsManagerWidget::moveStepUp()
{
    int currentRow = list->currentRow();
    if (currentRow == 0) return;
    QListWidgetItem *stepItem = list->takeItem( currentRow );
    if (!stepItem) return;

    list->insertItem( currentRow - 1, stepItem);
    list->setCurrentRow( currentRow - 1);
    updateStepsOrder();
}

void StepsManagerWidget::moveStepDown()
{
    int currentRow = list->currentRow();
    if (currentRow == list->count() - 1) return;
    QListWidgetItem *stepItem = list->takeItem( currentRow );
    if (!stepItem) return;

    list->insertItem( currentRow + 1, stepItem);
    list->setCurrentRow( currentRow + 1);
    updateStepsOrder();
}

void StepsManagerWidget::updateStepsOrder()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    for (int i = 0; i < list->count(); i++)
    {
        RamStep *step = project->step( list->item(i)->data(Qt::UserRole).toString() );
        if (!step) continue;
        step->setOrder(i);
        step->update();
    }
}
