#include "sequencesmanagerwidget.h"

SequencesManagerWidget::SequencesManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    sequenceWidget = new SequenceEditWidget(this);
    this->setWidget(sequenceWidget);
    sequenceWidget->setEnabled(false);

    this->setRole(RamUser::ProjectAdmin);

    connect(Ramses::instance(), &Ramses::projectChanged, this, &SequencesManagerWidget::changeProject);
}

void SequencesManagerWidget::currentDataChanged(QVariant data)
{
    sequenceWidget->setEnabled(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamSequence *sequence = project->sequence( data.toString() );
    if (!sequence) return;

    sequenceWidget->setSequence(sequence);
    sequenceWidget->setEnabled(true);
}

void SequencesManagerWidget::createItem()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    project->createSequence();
}

void SequencesManagerWidget::removeItem(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->removeSequence(data.toString());
}

void SequencesManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    list->blockSignals(true);
    list->clear();
    sequenceWidget->setSequence(nullptr);
    list->blockSignals(false);

    if (!project) return;

    //add steps
    foreach(RamSequence *seq, project->sequences()) newSequence(seq);
    _projectConnections << connect(project, &RamProject::newSequence, this, &SequencesManagerWidget::newSequence);
    _projectConnections << connect(project, SIGNAL(sequenceRemoved(QString)), this, SLOT(removeSequence(QString)));

    this->setEnabled(true);
}

void SequencesManagerWidget::newSequence(RamSequence *seq)
{
    if (!seq) return;

    if (seq->uuid() != "")
    {
        QListWidgetItem *seqItem = new QListWidgetItem(seq->name());
        seqItem->setData(Qt::UserRole, seq->uuid());
        this->addItem(seqItem);
        connect(seq, &RamSequence::removed, this, &SequencesManagerWidget::removeSequence);
        connect(seq, &RamSequence::changed, this, &SequencesManagerWidget::sequenceChanged);
    }
}

void SequencesManagerWidget::removeSequence(RamObject *seq)
{
    removeData(seq->uuid());
}

void SequencesManagerWidget::sequenceChanged(RamObject *seq)
{
    updateItem(seq->uuid(), seq->name());
}