#include "sequencelistmanagerwidget.h"

#include "ramsequence.h"
#include "ramses.h"

SequenceListManagerWidget::SequenceListManagerWidget(QWidget *parent):
    ObjectListManagerWidget<RamSequence*,RamProject*>(
        "Sequences",
        QIcon(":icons/sequence"),
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(changeProject(RamProject*)));
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamSequence*,RamProject*>::RemoveObjects);
    m_listEditWidget->setSortable(true);
}

RamSequence *SequenceListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return nullptr;
    RamSequence *s = new RamSequence(
                "NEW",
                "New Sequence",
                project);
    project->sequences()->append(s);
    s->edit();
    return s;
}

void SequenceListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( project->sequences() );
}
