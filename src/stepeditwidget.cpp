#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    typeBox->setItemData(0, "pre");
    typeBox->setItemData(1, "asset");
    typeBox->setItemData(2, "shot");
    typeBox->setItemData(3, "post");

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &StepEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::log, this, &StepEditWidget::dbiLog);

    this->setEnabled(false);
}

RamStep *StepEditWidget::step() const
{
    return _step;
}

void StepEditWidget::setStep(RamStep *step)
{
    disconnect(_currentStepConnection);

    _step = step;
    nameEdit->setText("");
    shortNameEdit->setText("");
    typeBox->setCurrentIndex(1);
    this->setEnabled(false);

    if (!step) return;

    nameEdit->setText(step->name());
    shortNameEdit->setText(step->shortName());

    if (step->type() == RamStep::PreProduction) typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) typeBox->setCurrentIndex(3);

    this->setEnabled(Ramses::instance()->isAdmin());

    _currentStepConnection = connect(step, &RamStep::destroyed, this, &StepEditWidget::stepDestroyed);
}

void StepEditWidget::update()
{
    if (!_step) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _step->setName(nameEdit->text());
    _step->setShortName(shortNameEdit->text());
    _step->setType(typeBox->currentData().toString());

    _step->update();

    this->setEnabled(true);

}

void StepEditWidget::revert()
{
    setStep(_step);
}

bool StepEditWidget::checkInput()
{
    if (!_step) return false;

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

void StepEditWidget::stepDestroyed(QObject */*o*/)
{
    setStep(nullptr);
}

void StepEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
