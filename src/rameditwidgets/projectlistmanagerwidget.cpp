#include "projectlistmanagerwidget.h"

ProjectListManagerWidget::ProjectListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->projects(),
        new ProjectEditWidget(),
        "Projects",
        parent )
{
}

void ProjectListManagerWidget::createObject()
{
    Ramses::instance()->createProject();
}

