#include "sequenceeditwidget.h"

#include "qcheckbox.h"
#include "ramsequence.h"
#include "ramshot.h"

SequenceEditWidget::SequenceEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

SequenceEditWidget::SequenceEditWidget(RamSequence *sequence, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(sequence);
}

RamSequence *SequenceEditWidget::sequence() const
{
    return m_sequence;
}

void SequenceEditWidget::reInit(RamObject *o)
{
    m_sequence = qobject_cast<RamSequence*>(o);
    if (m_sequence)
    {
        QSignalBlocker b1(ui_resolutionWidget);
        QSignalBlocker b2(ui_framerateWidget);

        ui_shotsList->setObjectModel( m_sequence->project()->shots() );
        ui_shotsList->setFilter(m_sequence);
        ui_colorSelector->setColor(m_sequence->color());
        ui_resolutionBox->setChecked(m_sequence->overrideResolution());
        ui_framerateBox->setChecked(m_sequence->overrideFramerate());
        ui_resolutionWidget->setEnabled(m_sequence->overrideResolution());
        ui_framerateWidget->setEnabled(m_sequence->overrideFramerate());
        ui_resolutionWidget->setHeight(m_sequence->height());
        ui_resolutionWidget->setWidth(m_sequence->width());
        ui_framerateWidget->setFramerate(m_sequence->framerate());

    }
    else
    {
        ui_resolutionWidget->setHeight(1080);
        ui_resolutionWidget->setWidth(1920);
        ui_framerateWidget->setFramerate(24.0);
        ui_resolutionBox->setChecked(false);
        ui_framerateBox->setChecked(false);
        ui_resolutionWidget->setEnabled(false);
        ui_framerateWidget->setEnabled(false);
        ui_shotsList->setObjectModel(nullptr);
        ui_colorSelector->setColor(QColor(67,67,67));
    }
}

void SequenceEditWidget::setColor(QColor c)
{
    if (!m_sequence || m_reinit) return;
    m_sequence->setColor(c);
}

void SequenceEditWidget::setOverrideResolution(bool r)
{
    ui_resolutionWidget->setEnabled(r);
    if (!m_sequence || m_reinit) return;
    m_sequence->setOverrideResolution(r);

    QSignalBlocker b(ui_resolutionWidget);
    ui_resolutionWidget->setHeight(m_sequence->height());
    ui_resolutionWidget->setWidth(m_sequence->width());
}

void SequenceEditWidget::setOverrideFramerate(bool r)
{
    ui_framerateWidget->setEnabled(r);
    if (!m_sequence || m_reinit) return;
    m_sequence->setOverrideFramerate(r);

    QSignalBlocker b(ui_framerateWidget);
    ui_framerateWidget->setFramerate(m_sequence->framerate());
}

void SequenceEditWidget::setResolution(int w, int h)
{
    m_sequence->setWidth(w);
    m_sequence->setHeight(h);
}

void SequenceEditWidget::setFramerate(qreal f)
{
    m_sequence->setFramerate(f);
}

void SequenceEditWidget::createShot()
{
    if (!m_sequence || m_reinit) return;
    RamShot *shot = new RamShot(
                "NEW",
                "New Shot",
                m_sequence);
    //m_sequence->project()->shots()->appendObject(shot->uuid());
    shot->edit();
}

void SequenceEditWidget::setupUi()
{
    QLabel *colorLabel = new QLabel("Color", this);
    ui_mainFormLayout->addWidget(colorLabel, 3, 0);

    ui_colorSelector = new DuColorSelector(this);
    ui_mainFormLayout->addWidget(ui_colorSelector, 3, 1);

    ui_resolutionBox = new QCheckBox("Override resolution", this);
    ui_mainFormLayout->addWidget(ui_resolutionBox, 4, 0);

    ui_resolutionWidget = new ResolutionWidget(this);
    ui_resolutionWidget->setEnabled(false);
    ui_mainFormLayout->addWidget(ui_resolutionWidget, 4, 1);

    ui_framerateBox = new QCheckBox("Override framerate", this);
    ui_mainFormLayout->addWidget(ui_framerateBox, 5, 0);

    ui_framerateWidget = new FramerateWidget(this);
    ui_framerateWidget->setEnabled(false);
    ui_mainFormLayout->addWidget(ui_framerateWidget, 5, 1);

    ui_shotsList = new ObjectListWidget(true, RamUser::ProjectAdmin, this);
    ui_shotsList->setEditMode(ObjectListWidget::UnassignObjects);
    ui_shotsList->setTitle("Shots");
    ui_mainLayout->addWidget(ui_shotsList);
}

void SequenceEditWidget::connectEvents()
{
    connect(ui_shotsList, SIGNAL(add()), this, SLOT(createShot()));
    connect(ui_colorSelector, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));
    connect(ui_resolutionBox, &QCheckBox::clicked, this, &SequenceEditWidget::setOverrideResolution);
    connect(ui_framerateBox, &QCheckBox::clicked, this, &SequenceEditWidget::setOverrideFramerate);
    connect(ui_resolutionWidget, &ResolutionWidget::resolutionChanged, this, &SequenceEditWidget::setResolution);
    connect(ui_framerateWidget, &FramerateWidget::framerateChanged, this, &SequenceEditWidget::setFramerate);
}

