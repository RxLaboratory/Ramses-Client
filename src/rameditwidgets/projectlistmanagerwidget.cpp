#include "projectlistmanagerwidget.h"

#include "ramses.h"

ProjectListManagerWidget::ProjectListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget<RamProject *, int>(
        Ramses::instance()->projects(),
        "Projects",
        QIcon(":icons/project"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamProject *, int>::RemoveObjects);
}

RamProject *ProjectListManagerWidget::createObject()
{
    RamProject *project = new RamProject(
                "NEW",
                "New Project"
                );
    Ramses::instance()->projects()->append(project);
    project->edit();
    return project;
}

