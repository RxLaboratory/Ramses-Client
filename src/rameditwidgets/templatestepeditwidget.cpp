#include "templatestepeditwidget.h"
#include "duqf-widgets/duicon.h"

TemplateStepEditWidget::TemplateStepEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

TemplateStepEditWidget::TemplateStepEditWidget(RamTemplateStep *templateStep, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(templateStep);
}

RamTemplateStep *TemplateStepEditWidget::step() const
{
    return m_step;
}

void TemplateStepEditWidget::reInit(RamObject *o)
{
    m_step = qobject_cast<RamTemplateStep*>(o);
    if (m_step)
    {
        ui_colorSelector->setColor(m_step->color());

        if (m_step->type() == RamTemplateStep::PreProduction) ui_typeBox->setCurrentIndex(0);
        else if (m_step->type() == RamTemplateStep::AssetProduction) ui_typeBox->setCurrentIndex(1);
        else if (m_step->type() == RamTemplateStep::ShotProduction) ui_typeBox->setCurrentIndex(2);
        else if (m_step->type() == RamTemplateStep::PostProduction) ui_typeBox->setCurrentIndex(3);

        ui_veryEasyEdit->setValue( m_step->estimationVeryEasy() );
        ui_easyEdit->setValue( m_step->estimationEasy() );
        ui_mediumEdit->setValue( m_step->estimationMedium()  );
        ui_hardEdit->setValue( m_step->estimationHard()  );
        ui_veryHardEdit->setValue( m_step->estimationVeryHard()  );
        ui_estimationTypeBox->setCurrentIndex( m_step->estimationMethod() );

        updateEstimationSuffix();
    }
    else
    {
        ui_typeBox->setCurrentIndex(1);
        ui_colorSelector->setColor(QColor(25,25,25));

        ui_veryEasyEdit->setValue(0.2);
        ui_easyEdit->setValue(0.5);
        ui_mediumEdit->setValue(1.0);
        ui_hardEdit->setValue(2.0);
        ui_veryHardEdit->setValue(3.0);
        ui_estimationTypeBox->setCurrentIndex(0);
    }
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

void TemplateStepEditWidget::setType(int t)
{
    Q_UNUSED(t);
    if (!m_step || m_reinit) return;
    m_step->setType(ui_typeBox->currentData().toString());
}

void TemplateStepEditWidget::setColor(QColor c)
{
    if (!m_step || m_reinit) return;
    m_step->setColor(c);
}

void TemplateStepEditWidget::setEstimationType(int t)
{
    if (!m_step || m_reinit) return;
    if (t == 0)
        m_step->setEstimationMethod( RamTemplateStep::EstimatePerShot );
    else
        m_step->setEstimationMethod( RamTemplateStep::EstimatePerSecond );
}

void TemplateStepEditWidget::setVeryEasy(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationVeryEasy(e);
}

void TemplateStepEditWidget::setEasy(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationEasy(e);
}

void TemplateStepEditWidget::setMedium(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationMedium(e);
}

void TemplateStepEditWidget::setHard(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationHard(e);
}

void TemplateStepEditWidget::setVeryHard(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationVeryHard(e);
}

void TemplateStepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    ui_mainFormLayout->addWidget(typeLabel, 3, 0);

    ui_typeBox  = new DuComboBox(this);
    ui_typeBox->addItem(DuIcon(":/icons/project"), "       Pre-Production", "pre");
    ui_typeBox->addItem(DuIcon(":/icons/asset"), "       Asset Production", "asset");
    ui_typeBox->addItem(DuIcon(":/icons/shot"), "       Shot Production", "shot");
    ui_typeBox->addItem(DuIcon(":/icons/film"), "       Post-Production", "post");
    ui_mainFormLayout->addWidget(ui_typeBox, 3, 1);

    QLabel *colorLabel = new QLabel("Color", this);
    ui_mainFormLayout->addWidget(colorLabel, 4, 0);

    ui_colorSelector = new DuQFColorSelector(this);
    ui_mainFormLayout->addWidget(ui_colorSelector, 4, 1);

    ui_estimationLabel = new QLabel("Estimation", this);
    ui_estimationLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui_mainFormLayout->addWidget(ui_estimationLabel, 5, 0);

    ui_estimationWidget = new QWidget(this);
    QFormLayout *estimationLayout = new QFormLayout(ui_estimationWidget);
    estimationLayout->setContentsMargins(0,0,0,0);
    estimationLayout->setSpacing(3);

    ui_estimationTypeLabel = new QLabel("Method", this);

    ui_estimationTypeBox = new DuComboBox(this);
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

    ui_mainFormLayout->addWidget(ui_estimationWidget, 5, 1);

    ui_mainLayout->addStretch();
}

void TemplateStepEditWidget::connectEvents()
{
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    connect(ui_colorSelector, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));

    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setEstimationType(int)));
    connect(ui_veryEasyEdit, SIGNAL(valueChanged(double)), this, SLOT(setVeryEasy(double)));
    connect(ui_easyEdit, SIGNAL(valueChanged(double)), this, SLOT(setEasy(double)));
    connect(ui_mediumEdit, SIGNAL(valueChanged(double)), this, SLOT(setMedium(double)));
    connect(ui_hardEdit, SIGNAL(valueChanged(double)), this, SLOT(setHard(double)));
    connect(ui_veryHardEdit, SIGNAL(valueChanged(double)), this, SLOT(setVeryHard(double)));
}
