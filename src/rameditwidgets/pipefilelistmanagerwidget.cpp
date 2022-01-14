#include "pipefilelistmanagerwidget.h"

PipeFileListManagerWidget::PipeFileListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        "Pipe Types",
        QIcon(":icons/file"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *PipeFileListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    RamPipeFile *pf = new RamPipeFile(
                "NEW",
                project);
    project->pipeFiles()->append(pf);
    pf->edit();
    return pf;
}

void PipeFileListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( project->pipeFiles() );
}
