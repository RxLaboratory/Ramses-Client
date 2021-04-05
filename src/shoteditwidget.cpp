#include "shoteditwidget.h"

ShotEditWidget::ShotEditWidget(RamShot *shot, QWidget *parent) :
    ObjectEditWidget(shot, parent)
{
    setupUi();
    connectEvents();

    setShot(shot);
}

void ShotEditWidget::setShot(RamShot *shot)
{
    this->setEnabled(false);

    setObject(shot);
    _shot = shot;

    QSignalBlocker b1(framesBox);
    QSignalBlocker b2(secondsBox);
    QSignalBlocker b3(sequencesBox);

    //Reset values
    framesBox->setValue(0);
    secondsBox->setValue(0);
    sequencesBox->clear();

    if (!shot) return;

    secondsBox->setValue(_shot->duration());
    secondsChanged();

    // Load sequences
    RamProject *proj = project();
    if (!proj) return;

    for (RamSequence *seq: proj->sequences())
    {
        sequencesBox->addItem( seq->name(), seq->uuid());

        if (_shot->sequenceUuid() == seq->uuid())
            sequencesBox->setCurrentIndex( sequencesBox->count() -1 );
    }

    _objectConnections << connect( shot, &RamShot::changed, this, &ShotEditWidget::shotChanged);

    this->setEnabled(Ramses::instance()->isLead());
}

void ShotEditWidget::update()
{
    if (!_shot) return;

    if (!checkInput()) return;

    updating = true;

    _shot->setName(nameEdit->text());
    _shot->setShortName(shortNameEdit->text());
    _shot->setDuration(secondsBox->value());

    _shot->update();

    updating = false;
}

void ShotEditWidget::shotChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setShot(_shot);
}

void ShotEditWidget::framesChanged()
{
    RamProject *proj = project();
    if (!proj) return;

    secondsBox->setValue( framesBox->value() / proj->framerate() );

    update();
}

void ShotEditWidget::secondsChanged()
{
    RamProject *proj = project();
    if (!proj) return;

    framesBox->setValue( secondsBox->value() * proj->framerate() );

    update();
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

    sequencesBox = new QComboBox(this);
    mainFormLayout->addWidget(sequencesBox, 4, 1);
}

void ShotEditWidget::connectEvents()
{
    connect(secondsBox, SIGNAL(editingFinished()), this, SLOT(secondsChanged()));
    connect(framesBox, SIGNAL(editingFinished()), this, SLOT(framesChanged()));
    connect(sequencesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}

RamSequence *ShotEditWidget::sequence()
{
    if (!_shot) return nullptr;
    return Ramses::instance()->sequence( _shot->sequenceUuid() );
}

RamProject *ShotEditWidget::project()
{
    if (!_shot) return nullptr;
    RamSequence *seq = sequence();
    if (!seq) return nullptr;
    return Ramses::instance()->project(seq->projectUuid());
}
