#include "templatestepeditwidget.h"

TemplateStepEditWidget::TemplateStepEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);
}

TemplateStepEditWidget::TemplateStepEditWidget(RamStep *templateStep, QWidget *parent) :
    ObjectEditWidget(templateStep, parent)
{
    setupUi();
    connectEvents();

    setObject(templateStep);
}

RamStep *TemplateStepEditWidget::step() const
{
    return _step;
}

void TemplateStepEditWidget::setObject(RamObject *obj)
{
    RamStep *step = (RamStep*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(step);
    _step = step;

    QSignalBlocker b1(ui_typeBox);

    ui_typeBox->setCurrentIndex(1);

    if (!step) return;

    if (step->type() == RamStep::PreProduction) ui_typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) ui_typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) ui_typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) ui_typeBox->setCurrentIndex(3);

    this->setEnabled(Ramses::instance()->isAdmin());

    updateEstimationSuffix();
}

void TemplateStepEditWidget::update()
{
    if (!_step) return;

    updating = true;

    _step->setType(ui_typeBox->currentData().toString());

    ObjectEditWidget::update();

    updating = false;
}

void TemplateStepEditWidget::updateEstimationSuffix()
{
    if (ui_typeBox->currentIndex() == 0 || ui_typeBox->currentIndex() == 3)
    {
        ui_estimationWidget->hide();
        ui_estimationLabel->hide();
        return;
    }

    ui_estimationWidget->show();
    ui_estimationLabel->show();

    QString suffix;

    if (ui_typeBox->currentIndex() == 1)
    {
        ui_estimationTypeBox->hide();
        ui_estimationTypeLabel->hide();
        suffix = " days per asset.";
    }
    else if (ui_typeBox->currentIndex() == 2)
    {
        ui_estimationTypeBox->show();
        ui_estimationTypeLabel->show();
        if (ui_estimationTypeBox->currentIndex() == 0)
        {
            suffix = " days per shot.";
        }
        else
        {
            suffix = " days per second.";
        }
    }

    ui_veryEasyEdit->setSuffix(suffix);
    ui_easyEdit->setSuffix(suffix);
    ui_mediumEdit->setSuffix(suffix);
    ui_hardEdit->setSuffix(suffix);
    ui_veryHardEdit->setSuffix(suffix);

}

void TemplateStepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    mainFormLayout->addWidget(typeLabel, 2, 0);

    ui_typeBox  = new QComboBox(this);
    ui_typeBox->addItem(QIcon(":/icons/project"), "       Pre-Production", "pre");
    ui_typeBox->addItem(QIcon(":/icons/asset"), "       Asset Production", "asset");
    ui_typeBox->addItem(QIcon(":/icons/shot"), "       Shot Production", "shot");
    ui_typeBox->addItem(QIcon(":/icons/film"), "       Post-Production", "post");
    mainFormLayout->addWidget(ui_typeBox, 2, 1);

    ui_estimationLabel = new QLabel("Estimation", this);
    ui_estimationLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    mainFormLayout->addWidget(ui_estimationLabel, 3, 0);

    ui_estimationWidget = new QWidget(this);
    QFormLayout *estimationLayout = new QFormLayout(ui_estimationWidget);
    estimationLayout->setContentsMargins(0,0,0,0);
    estimationLayout->setSpacing(3);

    ui_estimationTypeLabel = new QLabel("Method", this);

    ui_estimationTypeBox = new QComboBox(this);
    ui_estimationTypeBox->addItem("Per shot");
    ui_estimationTypeBox->addItem("Per second");
    estimationLayout->addRow(ui_estimationTypeLabel, ui_estimationTypeBox);

    ui_veryEasyEdit = new AutoSelectDoubleSpinBox(this);
    ui_veryEasyEdit->setMinimum(0.1);
    ui_veryEasyEdit->setMaximum(100);
    ui_veryEasyEdit->setDecimals(1);
    ui_veryEasyEdit->setValue(0.2);
    estimationLayout->addRow("Very easy", ui_veryEasyEdit);

    ui_easyEdit = new AutoSelectDoubleSpinBox(this);
    ui_easyEdit->setMinimum(0.1);
    ui_easyEdit->setMaximum(100);
    ui_easyEdit->setDecimals(1);
    ui_easyEdit->setValue(0.5);
    estimationLayout->addRow("Easy",ui_easyEdit);

    ui_mediumEdit = new AutoSelectDoubleSpinBox(this);
    ui_mediumEdit->setMinimum(0.1);
    ui_mediumEdit->setMaximum(100);
    ui_mediumEdit->setDecimals(1);
    ui_mediumEdit->setValue(1);
    estimationLayout->addRow("Medium",ui_mediumEdit);

    ui_hardEdit = new AutoSelectDoubleSpinBox(this);
    ui_hardEdit->setMinimum(0.1);
    ui_hardEdit->setMaximum(100);
    ui_hardEdit->setDecimals(1);
    ui_hardEdit->setValue(2);
    estimationLayout->addRow("Hard",ui_hardEdit);

    ui_veryHardEdit = new AutoSelectDoubleSpinBox(this);
    ui_veryHardEdit->setMinimum(0.1);
    ui_veryHardEdit->setMaximum(100);
    ui_veryHardEdit->setDecimals(1);
    ui_veryHardEdit->setValue(3);
    estimationLayout->addRow("Very hard",ui_veryHardEdit);

    mainFormLayout->addWidget(ui_estimationWidget, 3, 1);

    mainLayout->addStretch();
}

void TemplateStepEditWidget::connectEvents()
{
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}
