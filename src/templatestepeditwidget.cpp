#include "templatestepeditwidget.h"

TemplateStepEditWidget::TemplateStepEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    typeBox->setItemData(0, "pre");
    typeBox->setItemData(1, "asset");
    typeBox->setItemData(2, "shot");
    typeBox->setItemData(3, "post");

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &TemplateStepEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &TemplateStepEditWidget::dbiLog);

    this->setEnabled(false);
}

RamStep *TemplateStepEditWidget::step() const
{
    return _step;
}

void TemplateStepEditWidget::setStep(RamStep *step)
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

    _currentStepConnection = connect(step, &RamStep::destroyed, this, &TemplateStepEditWidget::stepDestroyed);
}

void TemplateStepEditWidget::update()
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

void TemplateStepEditWidget::revert()
{
    setStep(_step);
}

bool TemplateStepEditWidget::checkInput()
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

void TemplateStepEditWidget::stepDestroyed(QObject */*o*/)
{
    setStep(nullptr);
}

void TemplateStepEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
