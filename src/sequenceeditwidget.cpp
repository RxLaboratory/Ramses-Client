#include "sequenceeditwidget.h"

SequenceEditWidget::SequenceEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _sequence = nullptr;

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &SequenceEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &SequenceEditWidget::dbiLog);

    this->setEnabled(false);
}

RamSequence *SequenceEditWidget::sequence() const
{
    return _sequence;
}

void SequenceEditWidget::setSequence(RamSequence *sequence)
{
    while (_sequenceConnections.count() > 0) disconnect( _sequenceConnections.takeLast() );

    _sequence = sequence;

    nameEdit->setText("");
    shortNameEdit->setText("");
    this->setEnabled(false);

    if (!sequence) return;

    nameEdit->setText(sequence->name());
    shortNameEdit->setText(sequence->shortName());

    _sequenceConnections << connect(sequence, &RamSequence::destroyed, this, &SequenceEditWidget::sequenceDestroyed);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void SequenceEditWidget::update()
{
    if (!_sequence) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _sequence->setName(nameEdit->text());
    _sequence->setShortName(shortNameEdit->text());

    _sequence->update();

    this->setEnabled(true);
}

void SequenceEditWidget::revert()
{
    setSequence(nullptr);
}

bool SequenceEditWidget::checkInput()
{
    if (!_sequence) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void SequenceEditWidget::sequenceDestroyed(QObject */*o*/)
{
    setSequence(nullptr);
}

void SequenceEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
