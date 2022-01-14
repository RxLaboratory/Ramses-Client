#include "steplistmanagerwidget.h"

StepListManagerWidget::StepListManagerWidget(QWidget *parent):
ObjectListManagerWidget(
    "Steps",
    QIcon(":icons/step"),
    parent )
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortable(true);

    // Create from template actions
    ui_createMenu = new RamObjectListMenu(false, this);
    ui_createMenu->addCreateButton();
    QToolButton *addButton = m_listEditWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    ui_createMenu->setList(Ramses::instance()->templateSteps());

    connect(ui_createMenu, SIGNAL(create()), this, SLOT(createObject()));
    connect(ui_createMenu, SIGNAL(assign(RamObject*)), this, SLOT(createFromTemplate(RamObject*)));
}

RamObject *StepListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;

    RamStep *step = new RamStep(
                "NEW",
                "New Step",
                project
                );
    project->steps()->append(step);
    step->edit();
    return step;
}

void StepListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->setList(nullptr);
    if (!project) return;
    this->setList( project->steps() );
}

void StepListManagerWidget::createFromTemplate(RamObject *stepObj)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamStep *templateStep = qobject_cast<RamStep*>( stepObj );
    if (!templateStep) return;
    RamStep *step = templateStep->createFromTemplate(project);
    project->steps()->append(step);
    step->edit();
}
