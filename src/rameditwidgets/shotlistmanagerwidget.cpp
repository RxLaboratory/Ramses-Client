#include "shotlistmanagerwidget.h"

ShotLIstManagerWidget::ShotLIstManagerWidget(QWidget *parent):
    ObjectListManagerWidget(
        new ShotEditWidget(),
        "Shots",
        parent)
{
    changeProject(Ramses::instance()->currentProject());
}

void ShotLIstManagerWidget::createObject()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (project->sequences().count() == 0 ) return;
    RamSequence *seq = project->sequence(currentFilter());
    if (!seq) seq = project->sequences().at(0);
    seq->createShot();
}

void ShotLIstManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    this->setList(nullptr);
    //this->clearFilters();

    if (!project) return;

    //this->addFilter("All shots", "all");

    // add filters
    //foreach(RamSequence *sequence, project->sequences()) newSequence(sequence);

    //filter("all");

    //filterBox->blockSignals(false);

    //_projectConnections << connect(project, SIGNAL(sequenceRemoved(RamSequence*)), this, SLOT(sequenceRemoved(RamSequence*)));
    //_projectConnections << connect(project, &RamProject::newSequence, this, &ShotLIstManagerWidget::newSequence);

    this->setEnabled(true);
}

/*void ShotLIstManagerWidget::filter(QString sequenceUuid)
{
    while (_sequencesConnections.count() > 0) disconnect( _sequencesConnections.takeLast() );

    list->blockSignals(true);
    list->clear();
    list->blockSignals(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    foreach(RamSequence *sequence, project->sequences())
        if (sequenceUuid == "all" || sequenceUuid == sequence->uuid()) addShots(sequence);

}*/
