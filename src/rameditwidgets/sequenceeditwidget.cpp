#include "sequenceeditwidget.h"

SequenceEditWidget::SequenceEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setSequence(nullptr);
}

SequenceEditWidget::SequenceEditWidget(RamSequence *sequence, QWidget *parent) :
    ObjectEditWidget(sequence, parent)
{
    setupUi();
    connectEvents();
    setSequence(sequence);
}

RamSequence *SequenceEditWidget::sequence() const
{
    return _sequence;
}

void SequenceEditWidget::setSequence(RamSequence *sequence)
{
    this->setEnabled(false);

    setObject(sequence);
    _sequence = sequence;

    QSignalBlocker b1(shotsList);

    //Reset values
    shotsList->clear();

    if (!sequence) return;

    // Load shots
    for( int i = 0; i < sequence->count(); i++) newShot(sequence->at(i));

    _objectConnections << connect(sequence, &RamSequence::changed, this, &SequenceEditWidget::sequenceChanged);
    _objectConnections << connect(sequence, &RamSequence::objectAdded, this, &SequenceEditWidget::newShot);
    _objectConnections << connect(sequence, &RamSequence::objectRemoved, this, &SequenceEditWidget::shotRemoved);

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
    setSequence(_sequence);
}

void SequenceEditWidget::newShot(RamObject *obj)
{
    shotsList->addObject(obj, _creatingShot);
    _creatingShot = false;
}

void SequenceEditWidget::shotRemoved(RamObject *shot)
{
    shotsList->removeObject(shot);
}

void SequenceEditWidget::addShot()
{
    if (!_sequence) return;
    _creatingShot = true;
    _sequence->createShot();
}

void SequenceEditWidget::removeShot(RamObject *o)
{
    if (!_sequence) return;
    o->remove();
}

void SequenceEditWidget::setupUi()
{
    shotsList = new SimpleObjectList(true, this);
    shotsList->setSortable(true);
    shotsList->setTitle("Shots");
    mainLayout->addWidget(shotsList);
}

void SequenceEditWidget::connectEvents()
{
    connect(shotsList, &SimpleObjectList::add, this, &SequenceEditWidget::addShot);
    connect(shotsList, &SimpleObjectList::objectRemoved, this, &SequenceEditWidget::removeShot);
}
