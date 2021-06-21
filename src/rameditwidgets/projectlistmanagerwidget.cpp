#include "projectlistmanagerwidget.h"

ProjectListManagerWidget::ProjectListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->projects(),
        new ProjectEditWidget(),
        "Projects",
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void ProjectListManagerWidget::createObject()
{
    RamProject *project = new RamProject(
                "NEW",
                "New Project"
                );
    Ramses::instance()->projects()->append(project);
    editObject(project);
}

