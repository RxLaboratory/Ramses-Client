#include "sequencelistmanagerwidget.h"

SequenceListManagerWidget::SequenceListManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new SequenceEditWidget(parent),
        "Sequences",
        parent)
{
    changeProject(Ramses::instance()->currentProject());
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &SequenceListManagerWidget::changeProject);
}

void SequenceListManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamSequence *s = new RamSequence(
                "NEW",
                project,
                "New Sequence");
    project->sequences()->append(s);
}

void SequenceListManagerWidget::changeProject(RamProject *project)
{
    // empty list
    this->clear();
    if (!project) return;
    this->setList( qobject_cast<RamObjectList*>( project->sequences() ) );
}
