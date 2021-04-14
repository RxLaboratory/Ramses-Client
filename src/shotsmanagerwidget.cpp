#include "shotsmanagerwidget.h"

ShotsManagerWidget::ShotsManagerWidget(QWidget *parent):
    ListManagerWidget(parent)
{
    shotWidget = new ShotEditWidget(nullptr, this);
    this->setWidget(shotWidget);

    this->setRole(RamUser::Lead);

    list->setDragEnabled(true);

    // Add order buttons
    upButton = new QToolButton(this);
    upButton->setIcon(QIcon(":/icons/move-up"));
    downButton = new QToolButton(this);
    downButton->setIcon(QIcon(":/icons/move-down"));
    buttonsLayout->insertWidget(2, downButton);
    buttonsLayout->insertWidget(2, upButton);

    changeProject(Ramses::instance()->currentProject());

    connect(Ramses::instance(), &Ramses::projectChanged, this, &ShotsManagerWidget::changeProject);
    connect(this, &ListManagerWidget::filterChanged, this, &ShotsManagerWidget::filter);
    connect(upButton, &QToolButton::clicked, this, &ShotsManagerWidget::moveShotUp);
    connect(downButton, &QToolButton::clicked, this, &ShotsManagerWidget::moveShotDown);
    connect(list, &DuQFListWidget::itemDropped, this, &ShotsManagerWidget::updateShotsOrder);
}

void ShotsManagerWidget::currentDataChanged(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamShot *shot = project->shot( data.toString() );

    if (!shot) return;

    shotWidget->setShot(shot);
}

void ShotsManagerWidget::createItem()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (project->sequences().count() == 0 ) return;
    RamSequence *seq = project->sequence(currentFilter());
    if (!seq) seq = project->sequences().at(0);
    seq->createShot();
}

void ShotsManagerWidget::removeItem(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->removeShot(data.toString());
}

void ShotsManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    list->blockSignals(true);
    filterBox->blockSignals(true);

    list->clear();
    shotWidget->setShot(nullptr);
    this->clearFilters();

    list->blockSignals(false);

    if (!project) return;

    this->addFilter("All shots", "all");

    // add filters
    foreach(RamSequence *sequence, project->sequences()) newSequence(sequence);

    filter("all");

    filterBox->blockSignals(false);

    _projectConnections << connect(project, SIGNAL(sequenceRemoved(RamSequence*)), this, SLOT(sequenceRemoved(RamSequence*)));
    _projectConnections << connect(project, &RamProject::newSequence, this, &ShotsManagerWidget::newSequence);

    this->setEnabled(true);
}

void ShotsManagerWidget::newShot(RamShot *shot)
{
    if (!shot) return;
    if (shot->uuid() != "")
    {
        QListWidgetItem *shotItem = new QListWidgetItem(shot->name());
        shotItem->setData(Qt::UserRole, shot->uuid());
        this->addItem(shotItem);
        connect(shot, &RamShot::changed, this, &ShotsManagerWidget::shotChanged);
    }
}

void ShotsManagerWidget::shotChanged()
{
    RamShot *s = (RamShot*)QObject::sender();
    updateItem(s->uuid(), s->name());
}

void ShotsManagerWidget::shotRemoved(RamShot *s)
{
    removeData(s->uuid());
}

void ShotsManagerWidget::filter(QString sequenceUuid)
{
    while (_sequencesConnections.count() > 0) disconnect( _sequencesConnections.takeLast() );

    list->blockSignals(true);
    list->clear();
    list->blockSignals(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    foreach(RamSequence *sequence, project->sequences())
        if (sequenceUuid == "all" || sequenceUuid == sequence->uuid()) addShots(sequence);

}

void ShotsManagerWidget::addShots(RamSequence *sequence)
{
    foreach(RamShot *shot, sequence->shots())
    {
        newShot(shot);
    }
    _sequencesConnections << connect(sequence, &RamSequence::newShot, this, &ShotsManagerWidget::newShot);
    _sequencesConnections << connect(sequence, SIGNAL(shotRemoved(RamShot*)), this, SLOT(shotRemoved(RamShot*)));

}

void ShotsManagerWidget::sequenceChanged()
{
    RamSequence *sequence = (RamSequence*)sender();
    this->updateFilterName(sequence->name(), sequence->uuid());
}

void ShotsManagerWidget::sequenceRemoved(RamSequence *seq)
{
    this->removeFilter(seq->uuid());
}

void ShotsManagerWidget::newSequence(RamSequence *sequence)
{
    _projectConnections << connect(sequence, &RamSequence::changed, this, &ShotsManagerWidget::sequenceChanged);
    this->addFilter(sequence->name(), sequence->uuid());
}

void ShotsManagerWidget::moveShotUp()
{
    int currentRow = list->currentRow();
    if (currentRow == 0) return;
    QListWidgetItem *shotItem = list->takeItem( currentRow );
    if (!shotItem) return;

    list->insertItem( currentRow - 1, shotItem);
    list->setCurrentRow( currentRow - 1);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamShot *shot = project->shot(shotItem->data(Qt::UserRole).toString());
    if (!shot) return;

    shot->setOrder(currentRow -1);
    shot->update();
}

void ShotsManagerWidget::moveShotDown()
{
    int currentRow = list->currentRow();
    if (currentRow == list->count() - 1) return;
    QListWidgetItem *shotItem = list->takeItem( currentRow );
    if (!shotItem) return;

    list->insertItem( currentRow + 1, shotItem);
    list->setCurrentRow( currentRow + 1);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamShot *shot = project->shot(shotItem->data(Qt::UserRole).toString());
    if (!shot) return;

    shot->setOrder(currentRow + 1);
    shot->update();
}

void ShotsManagerWidget::updateShotsOrder()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    for (int i = 0; i < list->count(); i++)
    {
        RamShot *shot = project->shot( list->item(i)->data(Qt::UserRole).toString() );
        if (!shot) continue;
        shot->setOrder(i);
        shot->update();
    }
}
