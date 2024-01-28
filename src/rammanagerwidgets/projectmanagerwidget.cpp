#include "projectmanagerwidget.h"

#include "ramses.h"
#include "duqf-widgets/duicon.h"

ProjectManagerWidget::ProjectManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->projects(),
        "Projects",
        DuIcon(":icons/project"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamProject *ProjectManagerWidget::createObject()
{
    RamProject *project = new RamProject(
                "NEW",
                "New Project"
                );
    //Ramses::instance()->projects()->append(project);
    project->edit();
    return project;
}

