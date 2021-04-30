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
    return _sequence;
}

void SequenceEditWidget::setObject(RamObject *obj)
{
    RamSequence *sequence = qobject_cast<RamSequence*>(obj);

    this->setEnabled(false);

    ObjectEditWidget::setObject(sequence);
    _sequence = sequence;

    QSignalBlocker b1(shotsList);

    //Reset values
    shotsList->clear(sequence);

    if (!sequence) return;

    // Load shots

    _objectConnections << connect(sequence, &RamSequence::changed, this, &SequenceEditWidget::sequenceChanged);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void SequenceEditWidget::update()
{
    if (!_sequence) return;

    updating = true;

    if (!checkInput()) return;

    _sequence->setName(nameEdit->text());
    _sequence->setShortName(shortNameEdit->text());

    _sequence->update();

    updating = false;
}

void SequenceEditWidget::sequenceChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_sequence);
}

void SequenceEditWidget::createShot()
{qDebug() << _sequence;
    if (!_sequence) return;
    sequence()->createShot();
}

void SequenceEditWidget::setupUi()
{
    shotsList = new ObjectListEditWidget(true, this);
    shotsList->setSortable(true);
    shotsList->setTitle("Shots");
    mainLayout->addWidget(shotsList);
}

void SequenceEditWidget::connectEvents()
{
    connect(shotsList, &ObjectListEditWidget::add, this, &SequenceEditWidget::createShot);
}

