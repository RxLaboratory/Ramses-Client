#include "stepmanagerwidget.h"

#include "ramses.h"
#include "duwidgets/duicon.h"

StepManagerWidget::StepManagerWidget(QWidget *parent):
ObjectManagerWidget(
    "Steps",
    DuIcon(":icons/step"),
    parent )
{
    setProject(Ramses::i()->project());
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);

    // Create from template actions
    ui_createMenu = new RamObjectMenu(false, this);
    ui_createMenu->addCreateButton();
    QToolButton *addButton = ui_listWidget->addButton();
    addButton->setPopupMode(QToolButton::InstantPopup);
    addButton->setMenu(ui_createMenu);

    ui_createMenu->setObjectModel(Ramses::i()->templateSteps());

    connect(ui_createMenu, &RamObjectMenu::createTriggered, this, &StepManagerWidget::createObject);
    connect(ui_createMenu, &RamObjectMenu::assigned, this, &StepManagerWidget::createFromTemplate);
}

RamStep *StepManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;

    RamStep *step = new RamStep(
                "NEW",
                "New Step",
                project
                );
    step->edit();
    return step;
}

void StepManagerWidget::setProject(RamProject *project)
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

    RamProject *project = Ramses::i()->project();
    if (!project) return;
    RamStep *step = RamStep::createFromTemplate(templateStep, project);
    //project->steps()->appendObject(step->uuid());
    step->edit();
}
