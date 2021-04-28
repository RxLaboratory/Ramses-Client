#include "ramprojectwidget.h"

#include "mainwindow.h"

RamProjectWidget::RamProjectWidget(RamProject *project, QWidget *parent):
    RamObjectWidget(project, parent)
{
    _project = project;

    ProjectEditWidget *pw = new ProjectEditWidget(project, this);
    setEditWidget(pw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addProjectEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Admin);
    setIcon(":/icons/project");
}
