#include "pipefilemanagerwidget.h"

#include "duqf-widgets/duicon.h"
#include "ramses.h"

PipeFileManagerWidget::PipeFileManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Pipe Types",
        DuIcon(":icons/file"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamPipeFile *PipeFileManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    RamPipeFile *pf = new RamPipeFile(
                "NEW",
                project);
    pf->edit();
    return pf;
}

void PipeFileManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->pipeFiles() );
}
