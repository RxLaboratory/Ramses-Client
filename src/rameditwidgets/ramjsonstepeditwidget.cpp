#include "ramjsonstepeditwidget.h"

#include "duwidgets/duicon.h"
#include "duapp/duui.h"
#include "ramstep.h"
#include "ramses.h"

RamJsonStepEditWidget::RamJsonStepEditWidget(const QString &uuid, QWidget *parent):
    RamJsonObjectEditWidget(uuid, parent)
{
    setupUi();
    connectEvents();
    updateEstimationMethod();
}

QJsonObject RamJsonStepEditWidget::data() const
{
    QJsonObject obj = ramObjectData();
    obj.insert(RamTemplateStep::KEY_StepType, ui_typeBox->currentData().toString());
    obj.insert(RamTemplateStep::KEY_EstimationMethod, ui_estimationTypeBox->currentData().toString());
    obj.insert(RamTemplateStep::KEY_EstimationVeryEasy, ui_veryEasyEdit->value());
    obj.insert(RamTemplateStep::KEY_EstimationEasy, ui_easyEdit->value());
    obj.insert(RamTemplateStep::KEY_EstimationMedium, ui_mediumEdit->value());
    obj.insert(RamTemplateStep::KEY_EstimationHard, ui_hardEdit->value());
    obj.insert(RamTemplateStep::KEY_EstimationVeryHard, ui_veryHardEdit->value());
    obj.insert(RamTemplateStep::KEY_PublishSettings, ui_publishSettingsEdit->toPlainText());
    obj.insert(RamTemplateStep::KEY_PublishSettings, ui_publishSettingsEdit->toPlainText());

    QString mGroup = RamTemplateStep::ENUMVALUE_None;
    if (ui_estimationMultiplierCheckBox->isChecked() &&
        ui_typeBox->currentData().toString() == RamTemplateStep::ENUMVALUE_Shot &&
        exists()) {
        mGroup = ui_estimationMultiplierBox->currentUuid();
    }
    obj.insert(RamStep::KEY_EstimationMultiplyGroup, mGroup);
    return obj;
}

void RamJsonStepEditWidget::setData(const QJsonObject &obj, const QString &uuid)
{
    setRamObjectData(obj);
    setUuid(uuid);

    ui_typeBox->setCurrentData(obj.value(RamTemplateStep::KEY_StepType).toString( RamTemplateStep::ENUMVALUE_PreProd ));
    ui_estimationTypeBox->setCurrentData(obj.value(RamTemplateStep::KEY_EstimationMethod).toString( RamTemplateStep::ENUMVALUE_Shot ));
    ui_veryEasyEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationVeryEasy).toDouble( 0.2 ));
    ui_easyEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationEasy).toDouble( 0.5 ));
    ui_mediumEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationMedium).toDouble( 1 ));
    ui_hardEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationHard).toDouble( 2 ));
    ui_veryHardEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationVeryHard).toDouble( 3 ));
    ui_publishSettingsEdit->setText(obj.value(RamTemplateStep::KEY_PublishSettings).toString( ));

    updateEstimationMethod();

    QString mGroup = obj.value(RamStep::KEY_EstimationMultiplyGroup).toString(RamStep::ENUMVALUE_None);
    ui_estimationMultiplierBox->setObject( mGroup );
    ui_estimationMultiplierCheckBox->setChecked( mGroup != RamStep::ENUMVALUE_None );

    emit dataChanged(data());
}

void RamJsonStepEditWidget::updateUuid()
{

    if (ui_typeBox->currentData().toString() == RamTemplateStep::ENUMVALUE_Shot &&
        exists()) {
        ui_estimationMultiplierCheckBox->setVisible(true);
        ui_estimationMultiplierBox->setVisible(true);
    }
    else
    {
        ui_estimationMultiplierCheckBox->setVisible(false);
        ui_estimationMultiplierBox->setVisible(false);
    }

    RamProject *proj =  Ramses::i()->project();
    if (proj)
        ui_estimationMultiplierBox->setObjectModel(
            proj->assetGroups()
            );
}

void RamJsonStepEditWidget::updateEstimationMethod()
{
    QString type = ui_typeBox->currentData().toString();
    if (type == RamTemplateStep::ENUMVALUE_PreProd ||
        type == RamTemplateStep::ENUMVALUE_PostProd)
    {
        ui_tabWidget->setTabEnabled(1, false);
        return;
    }

    ui_tabWidget->setTabEnabled(1, true);

    if (type == RamTemplateStep::ENUMVALUE_Shot && exists())
    {
        ui_estimationMultiplierBox->setVisible(true);
        ui_estimationMultiplierCheckBox->setVisible(true);
    }
    else {
        ui_estimationMultiplierBox->setVisible(false);
        ui_estimationMultiplierCheckBox->setVisible(false);
    }

    QString suffix;

    if (ui_typeBox->currentData() == RamTemplateStep::ENUMVALUE_Asset)
    {
        ui_estimationTypeBox->hide();
        ui_estimationTypeLabel->hide();
        suffix = " days per asset.";
    }
    else if (ui_typeBox->currentData() == RamTemplateStep::ENUMVALUE_Shot)
    {
        ui_estimationTypeBox->show();
        ui_estimationTypeLabel->show();
        if (ui_estimationTypeBox->currentData() == RamTemplateStep::ENUMVALUE_Shot)
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

void RamJsonStepEditWidget::setupUi()
{
    ui_typeBox = new DuComboBox(this);
    ui_typeBox->addItem(DuIcon(":/icons/project"), "        Pre-Production", RamTemplateStep::ENUMVALUE_PreProd);
    ui_typeBox->addItem(DuIcon(":/icons/asset"), "        Asset Production", RamTemplateStep::ENUMVALUE_Asset);
    ui_typeBox->addItem(DuIcon(":/icons/shot"), "        Shot Production", RamTemplateStep::ENUMVALUE_Shot);
    ui_typeBox->addItem(DuIcon(":/icons/film"), "        Post-Production", RamTemplateStep::ENUMVALUE_PostProd);
    ui_propertiesWidget->attributesLayout()->insertRow(0, tr("Type"), ui_typeBox);

    ui_propertiesWidget->mainLayout()->addStretch(1);

    auto estimWidget = new QWidget(this);
    ui_tabWidget->insertTab(1, estimWidget, DuIcon(":/icons/stats"), "");
    ui_tabWidget->setTabToolTip(1, tr("Estimations"));

    auto estimLayout = DuUI::addBoxLayout(Qt::Vertical, estimWidget);

    estimLayout->addWidget(new QLabel("<b>" +
                                        tr("Estimations") +
                                        "</b>"));

    auto estimationLayout = DuUI::createFormLayout();
    DuUI::addBlock(estimationLayout, estimLayout);

    ui_estimationTypeLabel = new QLabel(tr("Method"));

    ui_estimationTypeBox = new DuComboBox(this);
    ui_estimationTypeBox->addItem("Per shot", RamTemplateStep::ENUMVALUE_Shot);
    ui_estimationTypeBox->addItem("Per second", RamTemplateStep::ENUMVALUE_Second);
    estimationLayout->addRow(ui_estimationTypeLabel, ui_estimationTypeBox);

    ui_veryEasyEdit = new AutoSelectDoubleSpinBox(this);
    ui_veryEasyEdit->setMinimum(0.1);
    ui_veryEasyEdit->setMaximum(100);
    ui_veryEasyEdit->setDecimals(1);
    ui_veryEasyEdit->setValue(0.2);
    estimationLayout->addRow(tr("Very easy"), ui_veryEasyEdit);

    ui_easyEdit = new AutoSelectDoubleSpinBox(this);
    ui_easyEdit->setMinimum(0.1);
    ui_easyEdit->setMaximum(100);
    ui_easyEdit->setDecimals(1);
    ui_easyEdit->setValue(0.5);
    estimationLayout->addRow(tr("Easy"),ui_easyEdit);

    ui_mediumEdit = new AutoSelectDoubleSpinBox(this);
    ui_mediumEdit->setMinimum(0.1);
    ui_mediumEdit->setMaximum(100);
    ui_mediumEdit->setDecimals(1);
    ui_mediumEdit->setValue(1);
    estimationLayout->addRow(tr("Medium"),ui_mediumEdit);

    ui_hardEdit = new AutoSelectDoubleSpinBox(this);
    ui_hardEdit->setMinimum(0.1);
    ui_hardEdit->setMaximum(100);
    ui_hardEdit->setDecimals(1);
    ui_hardEdit->setValue(2);
    estimationLayout->addRow(tr("Hard"),ui_hardEdit);

    ui_veryHardEdit = new AutoSelectDoubleSpinBox(this);
    ui_veryHardEdit->setMinimum(0.1);
    ui_veryHardEdit->setMaximum(100);
    ui_veryHardEdit->setDecimals(1);
    ui_veryHardEdit->setValue(3);
    estimationLayout->addRow(tr("Very hard"),ui_veryHardEdit);

    ui_estimationMultiplierCheckBox = new QCheckBox("Multiply by", this);
    ui_estimationMultiplierCheckBox->setToolTip("Multiply estimation by the number of assets in the specific asset group.");
    ui_estimationMultiplierBox = new RamObjectComboBox(this);
    ui_estimationMultiplierBox->setEditable(false);
    estimationLayout->addRow(ui_estimationMultiplierCheckBox, ui_estimationMultiplierBox);

    estimLayout->addStretch(1);

    auto publishWidget = new QWidget(this);
    ui_tabWidget->addTab(publishWidget, DuIcon(":/icons/settings"), "");
    ui_tabWidget->setTabToolTip(3, tr("Publish settings"));

    auto publishLayout = DuUI::addBoxLayout(Qt::Vertical, publishWidget);

    publishLayout->addWidget(new QLabel(
        "<b>"+tr("Publish settings")+"</b>"
        ));

    ui_publishSettingsEdit = new DuRichTextEdit(ui_tabWidget);
    ui_publishSettingsEdit->setUseMarkdown(false);
    ui_publishSettingsEdit->setPlaceholderText(tr("Publish settings"));
    publishLayout->addWidget(ui_publishSettingsEdit);
}

void RamJsonStepEditWidget::connectEvents()
{
    connect(ui_typeBox, &DuComboBox::dataActivated,
            this, &RamJsonStepEditWidget::updateEstimationMethod);
    connect(ui_estimationTypeBox, &DuComboBox::dataActivated,
            this, &RamJsonStepEditWidget::updateEstimationMethod);

    connect(ui_typeBox, &DuComboBox::dataActivated,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_estimationTypeBox, &DuComboBox::dataActivated,
            this, &RamJsonObjectEditWidget::emitEdited);

    connect(ui_publishSettingsEdit, &DuRichTextEdit::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_veryEasyEdit, &AutoSelectDoubleSpinBox::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_easyEdit, &AutoSelectDoubleSpinBox::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_mediumEdit, &AutoSelectDoubleSpinBox::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_hardEdit, &AutoSelectDoubleSpinBox::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);
    connect(ui_veryHardEdit, &AutoSelectDoubleSpinBox::editingFinished,
            this, &RamJsonObjectEditWidget::emitEdited);

    connect(ui_estimationMultiplierCheckBox, &QCheckBox::clicked,
            this, &RamJsonStepEditWidget::emitEdited);
    connect(ui_estimationMultiplierCheckBox, &QCheckBox::toggled,
            ui_estimationMultiplierBox, &RamObjectComboBox::setEnabled);

    connect(ui_estimationMultiplierBox, &RamObjectComboBox::dataActivated,
            this, &RamJsonObjectEditWidget::emitEdited);
}
