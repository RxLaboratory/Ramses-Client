#include "sequencelistmanagerwidget.h"

SequenceListManagerWidget::SequenceListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new SequenceEditWidget(parent),
        "Sequences",
        parent)
{
    this->setContainingType(RamObject::Sequence);
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &SequenceListManagerWidget::changeProject);
}

void SequenceListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->createSequence();
}

void SequenceListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( qobject_cast<RamObjectList*>( project->sequences() ) );
}
