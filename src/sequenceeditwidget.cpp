#include "sequenceeditwidget.h"

#include "dbisuspender.h"

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
    foreach( RamShot *shot, sequence->shots()) newShot(shot);

    _objectConnections << connect(sequence, &RamSequence::changed, this, &SequenceEditWidget::sequenceChanged);
    _objectConnections << connect(sequence, &RamSequence::newShot, this, &SequenceEditWidget::newShot);
    _objectConnections << connect(sequence, &RamSequence::shotRemovedFromSequence, this, &SequenceEditWidget::shotRemoved);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void SequenceEditWidget::update()
{
    if (!_sequence) return;

    updating = true;

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

void SequenceEditWidget::newShot(RamShot *shot)
{
    shotsList->addObject(shot);
}

void SequenceEditWidget::shotRemoved(RamShot *shot)
{
    shotsList->removeObject(shot);
}

void SequenceEditWidget::addShot()
{
    if (!_sequence) return;
    _sequence->createShot();
}

void SequenceEditWidget::removeShot(RamObject *o)
{
    if (!_sequence) return;
    o->remove();
}

void SequenceEditWidget::setupUi()
{
    QLabel *folderLabel = new QLabel(this);
    mainFormLayout->addWidget(folderLabel, 2, 0);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainFormLayout->addWidget(folderWidget, 2, 1);

    QLabel *shotsLabel = new QLabel("Shots", this);
    shotsLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
    mainFormLayout->addWidget(shotsLabel, 3, 0);

    shotsList = new SimpleObjectList(true, this);
    mainFormLayout->addWidget(shotsList);
}

void SequenceEditWidget::connectEvents()
{
    connect(shotsList, &SimpleObjectList::add, this, &SequenceEditWidget::addShot);
    connect(shotsList, &SimpleObjectList::objectRemoved, this, &SequenceEditWidget::removeShot);
}
