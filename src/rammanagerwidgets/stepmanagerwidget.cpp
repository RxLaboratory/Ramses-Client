#include "stepmanagerwidget.h"

#include "ramses.h"
#include "duqf-widgets/duicon.h"

StepManagerWidget::StepManagerWidget(QWidget *parent):
ObjectManagerWidget(
    "Steps",
    DuIcon(":icons/step"),
    parent )
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);

    // Create from template actions
    ui_createMenu = new RamObjectMenu(false, this);
    ui_createMenu->addCreateButton();
    QToolButton *addButton = ui_listWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    ui_createMenu->setObjectModel(Ramses::instance()->templateSteps());

    connect(ui_createMenu, &RamObjectMenu::createTriggered, this, &StepManagerWidget::createObject);
    connect(ui_createMenu, &RamObjectMenu::assigned, this, &StepManagerWidget::createFromTemplate);
}

RamStep *StepManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;

    RamStep *step = new RamStep(
                "NEW",
                "New Step",
                project
                );
    step->edit();
    return step;
}

void StepManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->setObjectModel(nullptr);
    if (!project) return;
    this->setObjectModel( project->steps() );
}

void StepManagerWidget::createFromTemplate(RamObject *templateStepObj)
{
    RamTemplateStep *templateStep = RamTemplateStep::c(templateStepObj);
    if (!templateStep) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamStep *step = RamStep::createFromTemplate(templateStep, project);
    //project->steps()->appendObject(step->uuid());
    step->edit();
}
