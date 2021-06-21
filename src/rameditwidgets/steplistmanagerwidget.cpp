#include "steplistmanagerwidget.h"

StepListManagerWidget::StepListManagerWidget(QWidget *parent):
ObjectListManagerWidget(
    new StepEditWidget(),
    "Steps",
    parent )
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &StepListManagerWidget::changeProject);
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
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
    this->clear();
    if (!project) return;
    this->setList( project->steps() );
}
