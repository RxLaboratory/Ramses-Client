#include "sequenceeditwidget.h"

SequenceEditWidget::SequenceEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(nullptr);
}

SequenceEditWidget::SequenceEditWidget(RamSequence *sequence, QWidget *parent) :
    ObjectEditWidget(sequence, parent)
{
    setupUi();
    connectEvents();
    setObject(sequence);
}

RamSequence *SequenceEditWidget::sequence() const
{
    return m_sequence;
}

void SequenceEditWidget::setObject(RamObject *obj)
{
    RamSequence *sequence = qobject_cast<RamSequence*>(obj);

    this->setEnabled(false);

    ObjectEditWidget::setObject(sequence);
    m_sequence = sequence;

    QSignalBlocker b1(ui_shotsList);

    ui_shotsList->setList(nullptr);

    if (!sequence) return;

    //Reset values
    ui_shotsList->setList(m_sequence->project()->shots());
    ui_shotsList->setFilter(sequence);
    ui_colorSelector->setColor(sequence->color());

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void SequenceEditWidget::update()
{
    if (!m_sequence) return;

    updating = true;

    if (!checkInput()) return;

    m_sequence->setColor( ui_colorSelector->color() );

    ObjectEditWidget::update();

    updating = false;
}

void SequenceEditWidget::createShot()
{
    if (!m_sequence) return;
    RamShot *shot = new RamShot(
                "NEW",
                m_sequence,
                "New Shot");
    m_sequence->project()->shots()->append(shot);
    shot->edit();
}

void SequenceEditWidget::setupUi()
{
    QLabel *colorLabel = new QLabel("Color", this);
    ui_mainFormLayout->addWidget(colorLabel, 3, 0);

    ui_colorSelector = new DuQFColorSelector(this);
    ui_mainFormLayout->addWidget(ui_colorSelector, 3, 1);

    ui_shotsList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, this);
    ui_shotsList->setEditMode(ObjectListEditWidget::UnassignObjects);
    ui_shotsList->setTitle("Shots");
    ui_mainLayout->addWidget(ui_shotsList);
}

void SequenceEditWidget::connectEvents()
{
    connect(ui_shotsList, SIGNAL(add()), this, SLOT(createShot()));
    connect(ui_colorSelector, SIGNAL(colorChanged(QColor)), this, SLOT(update()));

    monitorDbQuery("updateSequence");
}

