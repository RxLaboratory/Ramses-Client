#include "stepsmanagerwidget.h"

StepsManagerWidget::StepsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    stepWidget = new StepEditWidget(this);
    this->setWidget(stepWidget);
    stepWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    // Add menu
    assignMenu = new QMenu(this);
    actionCreateStep = new QAction("Create new step");
    assignMenu->addAction(actionCreateStep);
    assignMenu->addSeparator();
    addButton->setMenu(assignMenu);

    foreach(RamStep *step, Ramses::instance()->templateSteps()) newTemplateStep(step);

    connect(actionCreateStep, &QAction::triggered, this, &StepsManagerWidget::createStep);
    connect(Ramses::instance(), &Ramses::projectChanged, this, &StepsManagerWidget::changeProject);
    connect(Ramses::instance(), &Ramses::newTemplateStep, this, &StepsManagerWidget::newTemplateStep);
}

void StepsManagerWidget::currentDataChanged(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    foreach(RamStep *step, project->steps())
    {
        if (step->uuid() == data.toString())
        {
            stepWidget->setStep(step);
            stepWidget->setEnabled(true);
            return;
        }
    }
    stepWidget->setEnabled(false);
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

    disconnect(_currentProjectConnection);

    // empty list
    list->blockSignals(true);
    list->clear();
    stepWidget->setStep(nullptr);
    list->blockSignals(false);

    if (!project) return;

    //add steps
    foreach(RamStep *step, project->steps()) newStep(step);
    _currentProjectConnection = connect(project, &RamProject::newStep, this, &StepsManagerWidget::newStep);

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
        this->addItem(stepItem);
        connect(step, &RamStep::destroyed, this, &StepsManagerWidget::removeStep);
        connect(step, &RamStep::changed, this, &StepsManagerWidget::stepChanged);
    }
}

void StepsManagerWidget::removeStep(QObject *step)
{
    RamStep *s = (RamStep*)step;

    removeData(s->uuid());
}

void StepsManagerWidget::stepChanged()
{
    RamStep *step = (RamStep*)QObject::sender();
    updateItem(step->uuid(), step->name());
}

void StepsManagerWidget::newTemplateStep(RamStep *step)
{
    if (!step) return;
    if (step->uuid() == "") return;
    QAction *stepAction = new QAction(step->name());
    stepAction->setData(step->uuid());
    assignMenu->addAction(stepAction);
    connect(stepAction, &QAction::triggered, this, &StepsManagerWidget::assignStep);
    connect(step, &RamStep::destroyed, this, &StepsManagerWidget::removeTemplateStep);
    connect(step, &RamStep::changed, this, &StepsManagerWidget::templateStepChanged);
}

void StepsManagerWidget::removeTemplateStep(QObject *o)
{
    RamStep *s = (RamStep *)o;
    QList<QAction *> actions = assignMenu->actions();
    for (int i = list->count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == s->uuid()) actions[i]->deleteLater();
    }
}

void StepsManagerWidget::templateStepChanged()
{
    RamStep *s = (RamStep*)sender();
    QList<QAction *> actions = assignMenu->actions();
    for (int i = list->count() -1; i >= 0; i--)
    {
        if (actions[i]->data().toString() == s->uuid()) actions[i]->setText(s->name());
    }
}
