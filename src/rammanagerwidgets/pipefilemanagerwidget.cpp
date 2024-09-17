#include "pipefilemanagerwidget.h"

#include "duwidgets/duicon.h"
#include "ramses.h"

PipeFileManagerWidget::PipeFileManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Pipe Types",
        DuIcon(":icons/file"),
        parent)
{
    setProject(Ramses::i()->project());
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamPipeFile *PipeFileManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;
    RamPipeFile *pf = new RamPipeFile(
                "NEW",
                project);
    pf->edit();
    return pf;
}

void PipeFileManagerWidget::setProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->pipeFiles() );
}
