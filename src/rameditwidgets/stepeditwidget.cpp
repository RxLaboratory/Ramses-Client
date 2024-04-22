#include "stepeditwidget.h"
#include "duqf-widgets/duicon.h"
#include "ramapplication.h"
#include "ramproject.h"
#include "ramses.h"
#include "ramassetgroup.h"
#include "ramstep.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(s);
}

RamStep *StepEditWidget::step() const
{
    return m_step;
}

void StepEditWidget::reInit(RamObject *obj)
{
    bool same = m_step == obj;

    m_step = qobject_cast<RamStep*>( obj );

    QSignalBlocker b(ui_userBox);

    if (!same)
        connect(m_step->project(), &RamProject::estimationComputed,
                this, [this](){ reInit(m_step); });

    if (m_step)
    {                
        ui_colorSelector->setColor(m_step->color());
        ui_publishSettingsEdit->setPlainText(m_step->publishSettings());
        ui_generalSettingsEdit->setPlainText(m_step->customSettings());

        ui_folderWidget->setPath( m_step->path() );

        if (m_step->type() == RamStep::PreProduction) ui_typeBox->setCurrentIndex(0);
        else if (m_step->type() == RamStep::AssetProduction) ui_typeBox->setCurrentIndex(1);
        else if (m_step->type() == RamStep::ShotProduction) ui_typeBox->setCurrentIndex(2);
        else if (m_step->type() == RamStep::PostProduction) ui_typeBox->setCurrentIndex(3);

        m_applicationList->setObjectModel(m_step->applications());

        ui_estimationMultiplierBox->setObjectModel(m_step->project()->assetGroups());

        ui_veryEasyEdit->setValue( m_step->estimationVeryEasy() );
        ui_easyEdit->setValue( m_step->estimationEasy() );
        ui_mediumEdit->setValue( m_step->estimationMedium()  );
        ui_hardEdit->setValue( m_step->estimationHard()  );
        ui_veryHardEdit->setValue( m_step->estimationVeryHard()  );
        ui_estimationTypeBox->setCurrentIndex( m_step->estimationMethod() );

        if (m_step->estimationMultiplyGroup())
        {
            ui_estimationMultiplierBox->setEnabled(true);
            ui_estimationMultiplierCheckBox->setChecked(true);
            ui_estimationMultiplierBox->setObject( m_step->estimationMultiplyGroup() );
        }
        else
        {
            ui_estimationMultiplierBox->setEnabled(false);
            ui_estimationMultiplierCheckBox->setChecked(false);
            ui_estimationMultiplierBox->setCurrentIndex(-1);
        }

        updateEstimationSuffix();

        RamStep::Type stepType = m_step->type();
        if (stepType == RamStep::AssetProduction || stepType == RamStep::ShotProduction) {
            ui_completionWidget->show();
            ui_userBox->setObjectModel(m_step->project()->users(), "Users");
            changeUser(ui_userBox->currentObject());
        }
        else  {
            ui_completionWidget->hide();
            ui_userBox->setObjectModel(nullptr, "Users");
        }
    }
    else
    {
        ui_userBox->setObjectModel(nullptr, "Users");
        ui_typeBox->setCurrentIndex(1);
        ui_folderWidget->setPath("");
        m_applicationList->clear();
        ui_colorSelector->setColor(QColor(25,25,25));
        ui_publishSettingsEdit->setPlainText("");
        ui_generalSettingsEdit->setPlainText("");
        ui_typeBox->setCurrentText(0);

        ui_veryEasyEdit->setValue(0.2);
        ui_easyEdit->setValue(0.5);
        ui_mediumEdit->setValue(1.0);
        ui_hardEdit->setValue(2.0);
        ui_veryHardEdit->setValue(3.0);
        ui_estimationTypeBox->setCurrentIndex(0);
        ui_estimationMultiplierCheckBox->setChecked(false);
        ui_estimationMultiplierBox->setCurrentIndex(-1);
        ui_estimationMultiplierBox->setEnabled(false);
        ui_completionWidget->hide();
    }
}

void StepEditWidget::createApplication()
{
    if (!m_step) return;
    RamApplication *app = new RamApplication(
                "NEW",
                "New Application");
    m_step->applications()->appendObject(app->uuid());
    app->edit();
}

void StepEditWidget::updateEstimationSuffix()
{
    ui_estimationMultiplierCheckBox->hide();
    ui_estimationMultiplierBox->hide();

    if (ui_typeBox->currentIndex() == 0 || ui_typeBox->currentIndex() == 3)
    {
        ui_tabWidget->setTabEnabled(0, false);
        return;
    }

    ui_tabWidget->setTabEnabled(0, true);

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
        ui_estimationMultiplierCheckBox->show();
        ui_estimationMultiplierBox->show();
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

void StepEditWidget::setType(int t)
{
    Q_UNUSED(t);
    if (!m_step || m_reinit) return;
    m_step->setType(ui_typeBox->currentData().toString());
}

void StepEditWidget::setPublishSettings()
{
    if (!m_step || m_reinit) return;
    m_step->setPublishSettings(ui_publishSettingsEdit->toPlainText());
}

void StepEditWidget::setGeneralSettings()
{
    if (!m_step || m_reinit) return;
    m_step->setCustomSettings(ui_generalSettingsEdit->toPlainText());
}

void StepEditWidget::setColor(QColor c)
{
    if (!m_step || m_reinit) return;
    m_step->setColor(c);
}

void StepEditWidget::setEstimationType(int t)
{
    if (!m_step || m_reinit) return;
    if (t == 0)
        m_step->setEstimationMethod( RamStep::EstimatePerShot );
    else
        m_step->setEstimationMethod( RamStep::EstimatePerSecond );
}

void StepEditWidget::setVeryEasy(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationVeryEasy(e);
}

void StepEditWidget::setEasy(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationEasy(e);
}

void StepEditWidget::setMedium(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationMedium(e);
}

void StepEditWidget::setHard(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationHard(e);
}

void StepEditWidget::setVeryHard(double e)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationVeryHard(e);
}

void StepEditWidget::activateMultiplier(bool a)
{
    if (!m_step || m_reinit) return;
    if (a)
        m_step->setEstimationMultiplyGroup( RamAssetGroup::c( ui_estimationMultiplierBox->currentObject() ) );
    else
        m_step->setEstimationMultiplyGroup( nullptr );
}

void StepEditWidget::setMultiplier(RamObject *ag)
{
    if (!m_step || m_reinit) return;
    m_step->setEstimationMultiplyGroup( ag );
}

void StepEditWidget::changeUser(RamObject *userObj)
{
    while(ui_statesLayout->rowCount() > 0)
        ui_statesLayout->removeRow(0);
    while(ui_difficultiesLayout->rowCount() > 0)
        ui_difficultiesLayout->removeRow(0);

    if (!m_step) return;

    RamStep::Type stepType = m_step->type();
    if (stepType != RamStep::AssetProduction && stepType != RamStep::ShotProduction)
        return;

    RamUser *user = RamUser::c(userObj);

    int completion = m_step->completionRatio(user);
    ui_progressWidget->setCompletionRatio(completion);
    if (completion > 99)
        ui_completionLabel->setText("Finished!");
    else {
        ui_completionLabel->setText(
            QString("<i>Completion: <b>%1%</b> (%2 / %3 days)</i>")
                .arg(completion)
                .arg(int(m_step->daysSpent(user)))
                .arg(int(m_step->estimation(user)))
            );
    }

    const QVector<RamStep::StateCount> stateCount = m_step->stateCount(user);
    RamState *noState = Ramses::instance()->noState();
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
            l->setStyleSheet("QLabel { color: "+state->color().name() + "; }");
            ui_statesLayout->addRow( l, new QLabel(cStr, this) );
        }

        QString cStr = "<b>"+QString::number(total)+"</b> ";
        if (stepType == RamStep::AssetProduction) cStr += "assets";
        else cStr += "shots";

        ui_statesLayout->addRow( "Total", new QLabel(cStr, this) );
    }

    const QMap<RamStatus::Difficulty, int> difficulties = m_step->difficultyCount(user);
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
            ui_difficultiesLayout->addRow( l, new QLabel(cStr, this) );
        }

        QString cStr = "<b>"+QString::number(total)+"</b> ";
        if (stepType == RamStep::AssetProduction) cStr += "assets";
        else cStr += "shots";

        ui_difficultiesLayout->addRow( "Total", new QLabel(cStr, this) );
    }
}

void StepEditWidget::setupUi()
{
    auto appearanceWidget = new QWidget(this);
    appearanceWidget->setProperty("class", "duBlock");
    ui_mainLayout->addWidget(appearanceWidget);

    auto appaeranceLayout = new QGridLayout(appearanceWidget);
    appaeranceLayout->setSpacing(3);

    QLabel *typeLabel = new QLabel("Type", this);
    appaeranceLayout->addWidget(typeLabel, 0,0);

    ui_typeBox = new DuComboBox(this);
    ui_typeBox->addItem(DuIcon(":/icons/project"), "        Pre-Production", "pre");
    ui_typeBox->addItem(DuIcon(":/icons/asset"), "        Asset Production", "asset");
    ui_typeBox->addItem(DuIcon(":/icons/shot"), "        Shot Production", "shot");
    ui_typeBox->addItem(DuIcon(":/icons/film"), "        Post-Production", "post");
    appaeranceLayout->addWidget(ui_typeBox, 0, 1);

    QLabel *colorLabel = new QLabel("Color", this);
    appaeranceLayout->addWidget(colorLabel, 1, 0);

    ui_colorSelector = new DuQFColorSelector(this);
    appaeranceLayout->addWidget(ui_colorSelector, 1, 1);

    auto estimWidget = new QWidget(this);
    auto estimLayout = new QVBoxLayout(estimWidget);
    estimLayout->setContentsMargins(3,6,6,6);
    estimLayout->setSpacing(3);

    estimLayout->addWidget(new QLabel(
        "<b>"+tr("Estimations")+"</b>"
        ));

    ui_estimationWidget = new QWidget(ui_tabWidget);
    ui_estimationWidget->setProperty("class", "duBlock");
    estimLayout->addWidget(ui_estimationWidget);

    estimLayout->addWidget(new QLabel(
        "<b>"+tr("Status")+"</b>"
        ));

    ui_userBox = new RamObjectComboBox(this);
    estimLayout->addWidget(ui_userBox);

    ui_completionWidget = new QWidget(this);
    estimLayout->addWidget(ui_completionWidget);

    auto completionLayout = new QVBoxLayout(ui_completionWidget);
    completionLayout->setContentsMargins(0,0,0,0);
    completionLayout->setSpacing(5);

    ui_progressWidget = new ProgressWidget(this);
    completionLayout->addWidget(ui_progressWidget);

    ui_completionLabel = new QLabel(this);
    completionLayout->addWidget(ui_completionLabel);
    completionLayout->setAlignment(ui_completionLabel, Qt::AlignCenter);

    ui_statesWidget = new QWidget(ui_tabWidget);
    ui_statesLayout = new QFormLayout(ui_statesWidget);
    estimLayout->addWidget(ui_statesWidget);

    ui_difficultiesWidget = new QWidget(ui_tabWidget);
    ui_difficultiesLayout = new QFormLayout(ui_difficultiesWidget);
    estimLayout->addWidget(ui_difficultiesWidget);

    estimLayout->addStretch(1);

    QFormLayout *estimationLayout = new QFormLayout(ui_estimationWidget);
    estimationLayout->setContentsMargins(3,3,3,3);
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

    ui_estimationMultiplierCheckBox = new QCheckBox("Multiply by", this);
    ui_estimationMultiplierCheckBox->setToolTip("Multiply estimation by the number of assets in the specific asset group.");
    ui_estimationMultiplierBox = new RamObjectComboBox(this);
    ui_estimationMultiplierBox->setEnabled(false);
    estimationLayout->addRow(ui_estimationMultiplierCheckBox, ui_estimationMultiplierBox);

    ui_folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainLayout->addWidget(ui_folderWidget);

    ui_mainLayout->addStretch(1);

    ui_tabWidget->addTab(estimWidget, DuIcon(":/icons/stats-settings"), "");
    ui_tabWidget->setTabToolTip(2, tr("Estimations"));

    auto appDummyWidget = new QWidget(this);
    auto appDummyLayout = new QVBoxLayout(appDummyWidget);
    appDummyLayout->setContentsMargins(3, 3, 3, 3);

    auto appWidget = new QWidget(this);
    appWidget->setProperty("class", "duBlock");
    appDummyLayout->addWidget(appWidget);

    auto appLayout = new QVBoxLayout(appWidget);
    appLayout->setContentsMargins(3, 3, 3, 3);
    appLayout->setSpacing(3);

    m_applicationList = new ObjectListWidget(true, RamUser::ProjectAdmin, this);
    m_applicationList->setEditMode(ObjectListWidget::UnassignObjects);
    m_applicationList->setTitle("Applications");
    m_applicationList->setAssignList(Ramses::instance()->applications());
    appLayout->addWidget(m_applicationList);

    ui_tabWidget->addTab(appDummyWidget, DuIcon(":/icons/applications") ,"");
    ui_tabWidget->setTabToolTip(4, tr("Apps"));

    auto generalWidget = new QWidget(this);
    auto generalLayout = new QVBoxLayout(generalWidget);
    generalLayout->setContentsMargins(3,6,6,6);
    generalLayout->setSpacing(3);

    generalLayout->addWidget(new QLabel(
        "<b>"+tr("General settings")+"</b>"
        ));

    ui_generalSettingsEdit = new DuQFTextEdit(ui_tabWidget);
    ui_generalSettingsEdit->setProperty("class", "duBlock");
    ui_generalSettingsEdit->setUseMarkdown(false);
    ui_generalSettingsEdit->setPlaceholderText(tr("General settings"));
    generalLayout->addWidget(ui_generalSettingsEdit);

    ui_tabWidget->addTab(generalWidget, DuIcon(":/icons/settings"), "");
    ui_tabWidget->setTabToolTip(5, tr("General settings"));

    auto publishWidget = new QWidget(this);
    auto publishLayout = new QVBoxLayout(publishWidget);
    publishLayout->setContentsMargins(3,6,6,6);
    publishLayout->setSpacing(3);

    publishLayout->addWidget(new QLabel(
        "<b>"+tr("Publish settings")+"</b>"
        ));

    ui_publishSettingsEdit = new DuQFTextEdit(ui_tabWidget);
    ui_publishSettingsEdit->setProperty("class", "duBlock");
    ui_publishSettingsEdit->setUseMarkdown(false);
    ui_publishSettingsEdit->setPlaceholderText(tr("Publish settings"));
    publishLayout->addWidget(ui_publishSettingsEdit);

    ui_tabWidget->addTab(publishWidget, DuIcon(":/icons/settings"), "");
    ui_tabWidget->setTabToolTip(6, tr("Publish settings"));
}

void StepEditWidget::connectEvents()
{
    connect(ui_estimationMultiplierCheckBox, SIGNAL(clicked(bool)), ui_estimationMultiplierBox, SLOT(setEnabled(bool)));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));
    connect(ui_publishSettingsEdit, SIGNAL(editingFinished()), this, SLOT(setPublishSettings()));
    connect(ui_generalSettingsEdit, SIGNAL(editingFinished()), this, SLOT(setGeneralSettings()));

    connect(m_applicationList, SIGNAL(add()), this, SLOT(createApplication()));
    connect(ui_colorSelector, SIGNAL(colorChanged(QColor)), this, SLOT(setColor(QColor)));

    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setEstimationType(int)));
    connect(ui_veryEasyEdit, SIGNAL(valueChanged(double)), this, SLOT(setVeryEasy(double)));
    connect(ui_easyEdit, SIGNAL(valueChanged(double)), this, SLOT(setEasy(double)));
    connect(ui_mediumEdit, SIGNAL(valueChanged(double)), this, SLOT(setMedium(double)));
    connect(ui_hardEdit, SIGNAL(valueChanged(double)), this, SLOT(setHard(double)));
    connect(ui_veryHardEdit, SIGNAL(valueChanged(double)), this, SLOT(setVeryHard(double)));
    connect(ui_estimationMultiplierCheckBox, SIGNAL(clicked(bool)), this, SLOT(activateMultiplier(bool)));
    connect(ui_estimationMultiplierBox, &RamObjectComboBox::currentObjectChanged, this, &StepEditWidget::setMultiplier);

    connect(ui_userBox, &RamObjectComboBox::currentObjectChanged, this, &StepEditWidget::changeUser);
}
