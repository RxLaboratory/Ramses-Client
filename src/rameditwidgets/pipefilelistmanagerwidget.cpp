#include "pipefilelistmanagerwidget.h"

PipeFileListManagerWidget::PipeFileListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new PipeFileEditWidget(),
        "Pipe Types",
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &PipeFileListManagerWidget::changeProject);
}

void PipeFileListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamPipeFile *pf = new RamPipeFile(
                "NEW",
                project);
    project->pipeFiles()->append(pf);
}

void PipeFileListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( project->pipeFiles() );
}
