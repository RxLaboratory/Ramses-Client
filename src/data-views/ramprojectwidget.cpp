#include "ramprojectwidget.h"

RamProjectWidget::RamProjectWidget(RamProject *project, QWidget *parent):
    RamObjectWidget(project, parent)
{
    m_project = project;

    ProjectEditWidget *pw = new ProjectEditWidget(project, this);
    setEditWidget(pw);

    setUserEditRole(RamUser::Admin);
    setIcon(":/icons/project");

    completeUi();
}

void RamProjectWidget::exploreClicked()
{
    explore(Ramses::instance()->path(m_project));
}

void RamProjectWidget::completeUi()
{
    showExploreButton(true);
}
