#include "ramprojectwidget.h"

RamProjectWidget::RamProjectWidget(RamProject *project, QWidget *parent):
    RamObjectWidget(project, parent)
{
    _project = project;

    ProjectEditWidget *pw = new ProjectEditWidget(project, this);
    setEditWidget(pw);

    setUserEditRole(RamUser::Admin);
    setIcon(":/icons/project");
}
