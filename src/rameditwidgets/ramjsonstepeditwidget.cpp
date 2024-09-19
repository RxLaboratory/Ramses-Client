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
    setProject();
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

void RamJsonStepEditWidget::setData(const QJsonObject &obj)
{
    setRamObjectData(obj);

    ui_typeBox->setCurrentData( obj.value(RamTemplateStep::KEY_StepType).toString( RamTemplateStep::ENUMVALUE_PreProd ) );
    ui_estimationTypeBox->setCurrentData(obj.value(RamTemplateStep::KEY_EstimationMethod).toString( RamTemplateStep::ENUMVALUE_Shot ));
    ui_veryEasyEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationVeryEasy).toDouble( 0.2 ));
    ui_easyEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationEasy).toDouble( 0.5 ));
    ui_mediumEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationMedium).toDouble( 1 ));
    ui_hardEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationHard).toDouble( 2 ));
    ui_veryHardEdit->setValue(obj.value(RamTemplateStep::KEY_EstimationVeryHard).toDouble( 3 ));
    ui_publishSettingsEdit->setText(obj.value(RamTemplateStep::KEY_PublishSettings).toString( ));

    QString mGroup = obj.value(RamStep::KEY_EstimationMultiplyGroup).toString(RamStep::ENUMVALUE_None);
    ui_estimationMultiplierBox->setObject( mGroup );
    ui_estimationMultiplierCheckBox->setChecked( mGroup != RamStep::ENUMVALUE_None );

    updateEstimationMethod();

    emit dataChanged(data());
}

void RamJsonStepEditWidget::updateEstimationMethod()
{
    QString type = ui_typeBox->currentData().toString();
    if (type == RamTemplateStep::ENUMVALUE_PreProd ||
        type == RamTemplateStep::ENUMVALUE_PostProd)
    {
        ui_tabWidget->setTabEnabled(1, false);
        ui_estimWidget->setVisible(false);
        ui_estimLabel->setVisible(false);
        return;
    }

    ui_tabWidget->setTabEnabled(1, true);
    bool e = exists();

    if (type == RamStep::ENUMVALUE_Asset ||
        type == RamStep::ENUMVALUE_Shot)
    {
        ui_estimWidget->setVisible(e);
        ui_estimLabel->setVisible(e);
    }
    else
    {
        ui_estimWidget->setVisible(false);
        ui_estimLabel->setVisible(false);
    }

    if (type == RamTemplateStep::ENUMVALUE_Shot && e)
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

void RamJsonStepEditWidget::changeUser(RamObject *userObj)
{
    while(ui_statesLayout->rowCount() > 0)
        ui_statesLayout->removeRow(0);
    while(ui_difficultiesLayout->rowCount() > 0)
        ui_difficultiesLayout->removeRow(0);

    if (!exists())
        return;

    RamStep *step = RamStep::get(uuid());
    if (!step)
        return;

    RamStep::Type stepType = step->type();
    if (stepType != RamStep::AssetProduction && stepType != RamStep::ShotProduction)
        return;

    RamUser *user = RamUser::c(userObj);

    int completion = step->completionRatio(user);
    ui_progressWidget->setCompletionRatio(completion);
    if (completion > 99)
        ui_completionLabel->setText("Finished!");
    else {
        ui_completionLabel->setText(
            QString("<i>Completion: <b>%1%</b> (%2 / %3 days)</i>")
                .arg(completion)
                .arg(int(step->daysSpent(user)))
                .arg(int(step->estimation(user)))
            );
    }

    const QVector<RamStep::StateCount> stateCount = step->stateCount(user);
    RamState *noState = Ramses::i()->noState();
    float total = 0;

    for(const auto count: stateCount) {
        int c = count.count;
        if (c == 0)
            continue;

        RamState *state = count.state;
        if (state->is(noState))
            continue;

        total += c;
    }

    if (total > 0) {

        ui_statesLayout->setWidget( 0, QFormLayout::LabelRole, new QLabel("<b>"+tr("States:")+"</b>") );

        for(const auto count: stateCount) {
            float c = count.count;
            if (c == 0)
                continue;

            RamState *state = count.state;
            if (state->is(noState))
                continue;

            QString cStr = "<b>"+QString::number(c)+"</b> ";
            if (stepType == RamStep::AssetProduction) cStr += "assets (";
            else cStr += "shots (";
            cStr += QString::number(int(c/total*100)) + "%)";

            auto l = new QLabel(state->name(), this);
            l->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            l->setStyleSheet("QLabel { color: "+state->color().name() + "; }");
            auto l2 = new QLabel(cStr, this);
            l2->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            ui_statesLayout->addRow( l, l2 );
        }

        QString cStr = "<b>"+QString::number(total)+"</b> ";
        if (stepType == RamStep::AssetProduction) cStr += "assets";
        else cStr += "shots";

        auto l = new QLabel(cStr, this);
        l->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        ui_statesLayout->addRow( "Total", l );
    }

    const QMap<RamStatus::Difficulty, int> difficulties = step->difficultyCount(user);
    total = 0;

    QMapIterator<RamStatus::Difficulty, int> i(difficulties);
    while(i.hasNext()) {
        i.next();
        float c = i.value();
        if (c == 0)
            continue;
        total += c;
    }

    if (total > 0) {

        ui_difficultiesLayout->setWidget( 0, QFormLayout::LabelRole, new QLabel("<b>"+tr("Difficulty:")+"</b>") );

        i.toFront();
        while(i.hasNext()) {
            i.next();
            float c = i.value();
            if (c == 0)
                continue;

            QString cStr = "<b>"+QString::number(c)+"</b> ";
            if (stepType == RamStep::AssetProduction) cStr += "assets (";
            else cStr += "shots (";
            cStr += QString::number(int(c/total*100)) + "%)";

            QString label;

            switch(i.key()) {
            case RamStatus::VeryEasy:
                label = tr("Very easy");
                break;
            case RamStatus::Easy:
                label = tr("Easy");
                break;
            case RamStatus::Medium:
                label = tr("Medium");
                break;
            case RamStatus::Hard:
                label = tr("Hard");
                break;
            case RamStatus::VeryHard:
                label = tr("Very hard");
                break;
            }

            auto l = new QLabel(label, this);
            l->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            auto l2 = new QLabel(cStr, this);
            l2->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            ui_difficultiesLayout->addRow( l, l2 );
        }

        QString cStr = "<b>"+QString::number(total)+"</b> ";
        if (stepType == RamStep::AssetProduction) cStr += "assets";
        else cStr += "shots";

        auto l = new QLabel(cStr, this);
        l->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        ui_difficultiesLayout->addRow( tr("Total"), l );
    }
}

void RamJsonStepEditWidget::setupUi()
{
    setupMainTab();
    setupEstimTab();
    setupPublishTab();
}

void RamJsonStepEditWidget::setupMainTab()
{
    ui_typeBox = new DuComboBox(this);
    ui_typeBox->addItem(DuIcon(":/icons/project"), "        Pre-Production", RamTemplateStep::ENUMVALUE_PreProd);
    ui_typeBox->addItem(DuIcon(":/icons/asset"), "        Asset Production", RamTemplateStep::ENUMVALUE_Asset);
    ui_typeBox->addItem(DuIcon(":/icons/shot"), "        Shot Production", RamTemplateStep::ENUMVALUE_Shot);
    ui_typeBox->addItem(DuIcon(":/icons/film"), "        Post-Production", RamTemplateStep::ENUMVALUE_PostProd);
    ui_propertiesWidget->attributesLayout()->insertRow(0, tr("Type"), ui_typeBox);

    auto mainLayout = ui_propertiesWidget->mainLayout();

    ui_estimLabel = new QLabel(
        "<b>"+tr("Estimations")+"</b>"
        );
    mainLayout->addWidget(ui_estimLabel);

    auto estimLayout = DuUI::createBoxLayout(Qt::Vertical);
    ui_estimWidget = DuUI::addBlock(estimLayout, mainLayout);

    estimLayout->addWidget( new QLabel(
        "<b>"+tr("Status")+"</b>"
        ) );

    ui_userBox = new RamObjectComboBox(this);
    estimLayout->addWidget(ui_userBox);

    ui_completionWidget = new QWidget(this);
    estimLayout->addWidget(ui_completionWidget);

    auto completionLayout = DuUI::addBoxLayout(Qt::Vertical, estimLayout);

    ui_progressWidget = new ProgressWidget(this);
    completionLayout->addWidget(ui_progressWidget);

    ui_completionLabel = new QLabel(this);
    ui_completionLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    completionLayout->addWidget(ui_completionLabel);
    completionLayout->setAlignment(ui_completionLabel, Qt::AlignCenter);

    ui_statesWidget = new QWidget(ui_tabWidget);
    ui_statesWidget->setProperty("class", "transparent");
    ui_statesWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui_statesLayout = new QFormLayout(ui_statesWidget);
    estimLayout->addWidget(ui_statesWidget);

    ui_difficultiesWidget = new QWidget(ui_tabWidget);
    ui_difficultiesWidget->setProperty("class", "transparent");
    ui_difficultiesWidget->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui_difficultiesLayout = new QFormLayout(ui_difficultiesWidget);
    estimLayout->addWidget(ui_difficultiesWidget);

    mainLayout->addStretch(1);
}

void RamJsonStepEditWidget::setupEstimTab()
{
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
}

void RamJsonStepEditWidget::setupPublishTab()
{
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

void RamJsonStepEditWidget::setProject()
{
    RamProject *proj =  Ramses::i()->project();
    if (proj) {
        ui_estimationMultiplierBox->setObjectModel(
            proj->assetGroups()
            );
        ui_userBox->setObjectModel(
            proj->users(), "Users"
            );
        changeUser(ui_userBox->currentObject());
    }
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

    connect(ui_userBox, &RamObjectComboBox::currentObjectChanged, this, &RamJsonStepEditWidget::changeUser);
}
