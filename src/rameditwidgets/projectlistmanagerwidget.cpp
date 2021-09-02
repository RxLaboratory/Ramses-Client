#include "projectlistmanagerwidget.h"

ProjectListManagerWidget::ProjectListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->projects(),
        new ProjectEditWidget(),
        "Projects",
        QIcon(":icons/project"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *ProjectListManagerWidget::createObject()
{
    RamProject *project = new RamProject(
                "NEW",
                "New Project"
                );
    Ramses::instance()->projects()->append(project);
    editObject(project);
    return project;
}

