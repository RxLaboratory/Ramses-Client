#include "shoteditwidget.h"

ShotEditWidget::ShotEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();

    changeProject(Ramses::instance()->currentProject());

    connectEvents();
}

void ShotEditWidget::setShot(RamShot *shot)
{
    this->setEnabled(false);

    ObjectEditWidget::setObject(shot);
    _shot = shot;

    QSignalBlocker b1(framesBox);
    QSignalBlocker b2(secondsBox);
    QSignalBlocker b3(sequencesBox);
    QSignalBlocker b4(folderWidget);

    //Reset values
    framesBox->setValue(0);
    secondsBox->setValue(0);
    folderWidget->setPath("");
    sequencesBox->setCurrentIndex(-1);
    statusHistoryWidget->setItem(shot);

    if (!shot) return;

    secondsBox->setValue(_shot->duration());
    secondsChanged();
    folderWidget->setPath(Ramses::instance()->path(shot));

    // Set sequence
    sequencesBox->setObject( _shot->sequence() );

    this->setEnabled(Ramses::instance()->isLead());
}

void ShotEditWidget::setObject(RamObject *obj)
{
    RamShot *shot = qobject_cast<RamShot*>(obj);
    setShot(shot);
}

void ShotEditWidget::update()
{
    if (!_shot) return;

    if (!checkInput()) return;

    updating = true;

    _shot->setDuration(secondsBox->value());

    ObjectEditWidget::update();

    updating = false;
}

void ShotEditWidget::changeProject(RamProject *project)
{
    setShot(nullptr);
    if (!project)
    {
        sequencesBox->setList(nullptr);
        return;
    }
    sequencesBox->setList(project->sequences());
}

void ShotEditWidget::moveShot()
{
    if (!_shot) return;
    RamProject *proj = project();
    if (!proj) return;
    if (sequencesBox->currentIndex() >= 0)
        proj->moveShotToSequence(_shot, sequencesBox->currentUuid() );
}

void ShotEditWidget::framesChanged()
{
    RamProject *proj = project();
    if (!proj) return;

    secondsBox->setValue( framesBox->value() / proj->framerate() );
}

void ShotEditWidget::secondsChanged()
{
    RamProject *proj = project();
    if (!proj) return;

    framesBox->setValue( secondsBox->value() * proj->framerate() );
}

void ShotEditWidget::setupUi()
{
    //Duration
    QLabel *durationLabel = new QLabel("Duration", this);
    mainFormLayout->addWidget(durationLabel, 2, 0);

    secondsBox = new QDoubleSpinBox(this);
    secondsBox->setMinimum(0.0);
    secondsBox->setMaximum(14400.0);
    secondsBox->setSingleStep(0.1);
    secondsBox->setSuffix(" seconds");
    mainFormLayout->addWidget(secondsBox, 2, 1);

    framesBox = new QSpinBox(this);
    framesBox->setMinimum(0);
    framesBox->setMaximum(1728000);
    framesBox->setSingleStep(1);
    framesBox->setSuffix(" frames");
    mainFormLayout->addWidget(framesBox, 3, 1);

    QLabel *seqLabel = new QLabel("Sequence", this);
    mainFormLayout->addWidget(seqLabel, 4,0);

    sequencesBox = new RamObjectListComboBox(this);
    mainFormLayout->addWidget(sequencesBox, 4, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    statusHistoryWidget = new StatusHistoryWidget( this );
    mainLayout->addWidget(statusHistoryWidget);
}

void ShotEditWidget::connectEvents()
{
    connect(secondsBox, SIGNAL(editingFinished()), this, SLOT(secondsChanged()));
    connect(secondsBox, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(framesBox, SIGNAL(editingFinished()), this, SLOT(framesChanged()));
    connect(framesBox, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(sequencesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(moveShot()));
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ShotEditWidget::changeProject);
}

RamSequence *ShotEditWidget::sequence()
{
    if (!_shot) return nullptr;
    return _shot->sequence();
}

RamProject *ShotEditWidget::project()
{
    if (!_shot) return nullptr;
    return _shot->project();
}
