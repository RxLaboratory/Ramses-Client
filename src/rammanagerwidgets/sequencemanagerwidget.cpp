#include "sequencemanagerwidget.h"

#include "ramsequence.h"
#include "ramses.h"
#include "duwidgets/duicon.h"

SequenceManagerWidget::SequenceManagerWidget(QWidget *parent):
    ObjectManagerWidget(
        "Sequences",
        DuIcon(":icons/sequence"),
        parent)
{
    setProject(Ramses::i()->project());
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamSequence *SequenceManagerWidget::createObject()
{
    RamProject *project = Ramses::i()->project();
    if (!project) return nullptr;
    RamSequence *s = new RamSequence(
                "NEW",
                "New Sequence",
                project);
    s->edit();
    return s;
}

void SequenceManagerWidget::setProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setObjectModel( project->sequences() );
}
