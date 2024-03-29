#include "sequencemanagerwidget.h"

#include "ramsequence.h"
#include "ramses.h"
#include "duqf-widgets/duicon.h"

SequenceManagerWidget::SequenceManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Sequences",
        DuIcon(":icons/sequence"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamSequence *SequenceManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    RamSequence *s = new RamSequence(
                "NEW",
                "New Sequence",
                project);
    s->edit();
    return s;
}

void SequenceManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->sequences() );
}
