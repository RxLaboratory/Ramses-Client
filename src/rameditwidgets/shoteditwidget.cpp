#include "shoteditwidget.h"

ShotEditWidget::ShotEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

ShotEditWidget::ShotEditWidget(RamShot *shot, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(shot);
}

void ShotEditWidget::setObject(RamObject *obj)
{
    this->setEnabled(false);
    RamShot *shot = qobject_cast<RamShot*>(obj);

    ObjectEditWidget::setObject(shot);
    m_shot = shot;

    QSignalBlocker b1(ui_framesBox);
    QSignalBlocker b2(ui_secondsBox);
    QSignalBlocker b3(ui_sequencesBox);
    QSignalBlocker b4(ui_folderWidget);

    //Reset values
    ui_framesBox->setValue(0);
    ui_secondsBox->setValue(0);
    ui_folderWidget->setPath("");
    ui_sequencesBox->setCurrentIndex(-1);

    if (!shot) return;

    ui_secondsBox->setValue(m_shot->duration());
    secondsChanged();
    ui_folderWidget->setPath( shot->path() );

    // Set sequence
    RamProject *project = shot->project();
    ui_sequencesBox->setList(project->sequences());
    ui_sequencesBox->setObject( m_shot->sequence() );

    // Set assets
    ui_assetList->setList( m_shot->assets() );
    ui_assetList->setFilterList( project->assetGroups() );
    ui_assetList->setAssignList( project->assets() );

    this->setEnabled(Ramses::instance()->isLead());
}

void ShotEditWidget::update()
{
    if (!m_shot) return;

    if (!checkInput()) return;

    updating = true;

    m_shot->setDuration(ui_secondsBox->value());
    RamSequence *seq = qobject_cast<RamSequence*>( ui_sequencesBox->currentObject() );
    m_shot->setSequence(seq);

    ObjectEditWidget::update();

    updating = false;
}

void ShotEditWidget::framesChanged()
{
    RamProject *proj = m_shot->project();
    if (!proj) return;

    ui_secondsBox->setValue( ui_framesBox->value() / proj->framerate() );
}

void ShotEditWidget::secondsChanged()
{
    RamProject *proj = m_shot->project();
    if (!proj) return;

    ui_framesBox->setValue( ui_secondsBox->value() * proj->framerate() );
}

void ShotEditWidget::setupUi()
{
    //Duration
    QLabel *durationLabel = new QLabel("Duration", this);
    ui_mainFormLayout->addWidget(durationLabel, 3, 0);

    ui_secondsBox = new AutoSelectDoubleSpinBox(this);
    ui_secondsBox->setMinimum(0.0);
    ui_secondsBox->setMaximum(14400.0);
    ui_secondsBox->setSingleStep(0.1);
    ui_secondsBox->setSuffix(" seconds");
    ui_mainFormLayout->addWidget(ui_secondsBox, 3, 1);

    ui_framesBox = new AutoSelectSpinBox(this);
    ui_framesBox->setMinimum(0);
    ui_framesBox->setMaximum(1728000);
    ui_framesBox->setSingleStep(1);
    ui_framesBox->setSuffix(" frames");
    ui_mainFormLayout->addWidget(ui_framesBox, 4, 1);

    QLabel *seqLabel = new QLabel("Sequence", this);
    ui_mainFormLayout->addWidget(seqLabel, 5,0);

    ui_sequencesBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(ui_sequencesBox, 5, 1);

    ui_folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainLayout->addWidget( ui_folderWidget);

    ui_assetList = new ObjectListEditWidget(true, RamUser::Lead, this);
    ui_assetList->setEditMode(ObjectListEditWidget::UnassignObjects);
    ui_assetList->setEditable(true);
    ui_assetList->setSearchable(true);
    ui_assetList->setTitle("Assets");

    ui_mainLayout->addWidget(ui_assetList);
}

void ShotEditWidget::connectEvents()
{
    connect(ui_secondsBox, SIGNAL(editingFinished()), this, SLOT(secondsChanged()));
    connect(ui_secondsBox, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(ui_framesBox, SIGNAL(editingFinished()), this, SLOT(framesChanged()));
    connect(ui_framesBox, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(ui_sequencesBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}
