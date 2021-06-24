#include "stepeditwidget.h"

StepEditWidget::StepEditWidget(QWidget *parent) : ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);
}

StepEditWidget::StepEditWidget(RamStep *s, QWidget *parent) : ObjectEditWidget(s, parent)
{
    setupUi();
    connectEvents();

    setObject(s);
}

RamStep *StepEditWidget::step() const
{
    return m_step;
}

void StepEditWidget::setObject(RamObject *obj)
{
    RamStep *step = qobject_cast<RamStep*>( obj );
    this->setEnabled(false);

    ObjectEditWidget::setObject(step);
    m_step = step;

    QSignalBlocker b(ui_typeBox);
    QSignalBlocker b1(m_folderWidget);
    QSignalBlocker b2(m_userList);
    QSignalBlocker b3(m_applicationList);

    ui_typeBox->setCurrentIndex(1);
    m_folderWidget->setPath("");
    m_userList->clear();
    m_applicationList->clear();

    if (!step) return;

    m_folderWidget->setPath(Ramses::instance()->path(step));
    if (step->type() == RamStep::PreProduction) ui_typeBox->setCurrentIndex(0);
    else if (step->type() == RamStep::AssetProduction) ui_typeBox->setCurrentIndex(1);
    else if (step->type() == RamStep::ShotProduction) ui_typeBox->setCurrentIndex(2);
    else if (step->type() == RamStep::PostProduction) ui_typeBox->setCurrentIndex(3);

    m_userList->setList(step->users());
    m_applicationList->setList(step->applications());

    ui_estimationMultiplierBox->setList(step->project()->assetGroups());

    updateEstimationSuffix();

    this->setEnabled(Ramses::instance()->isProjectAdmin());   
}

void StepEditWidget::update()
{
    if(!m_step) return;

    updating = true;

    m_step->setType(ui_typeBox->currentData().toString());
    ObjectEditWidget::update();

    updating = false;
}

void StepEditWidget::createUser()
{
    if (!m_step) return;
    RamUser *user = new RamUser(
                "NEW",
                "John Doe");
    Ramses::instance()->users()->append(user);
    m_step->users()->append(user);
    user->edit();
}

void StepEditWidget::createApplication()
{
    if (!m_step) return;
    RamApplication *app = new RamApplication(
                "NEW",
                "New Application");
    Ramses::instance()->applications()->append(app);
    m_step->applications()->append(app);
    app->edit();
}

void StepEditWidget::updateEstimationSuffix()
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

void StepEditWidget::setupUi()
{
    QLabel *typeLabel = new QLabel("Type", this);
    ui_mainFormLayout->addWidget(typeLabel, 3,0);

    ui_typeBox = new QComboBox(this);
    ui_typeBox->addItem(QIcon(":/icons/project"), "        Pre-Production", "pre");
    ui_typeBox->addItem(QIcon(":/icons/asset"), "        Asset Production", "asset");
    ui_typeBox->addItem(QIcon(":/icons/shot"), "        Shot Production", "shot");
    ui_typeBox->addItem(QIcon(":/icons/film"), "        Post-Production", "post");
    ui_mainFormLayout->addWidget(ui_typeBox, 3, 1);

    ui_estimationLabel = new QLabel("Estimation", this);
    ui_estimationLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui_mainFormLayout->addWidget(ui_estimationLabel, 4, 0);

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

    ui_estimationMultiplierCheckBox = new QCheckBox("Multiply by", this);
    ui_estimationMultiplierCheckBox->setToolTip("Multiply estimation by the number of assets in the specific asset group.");
    ui_estimationMultiplierBox = new RamObjectListComboBox(this);
    ui_estimationMultiplierBox->setEnabled(false);
    estimationLayout->addRow(ui_estimationMultiplierCheckBox, ui_estimationMultiplierBox);

    ui_mainFormLayout->addWidget(ui_estimationWidget, 4, 1);

    m_folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainLayout->insertWidget(1, m_folderWidget);

    QTabWidget *tabWidget = new QTabWidget(this);

    m_userList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, tabWidget);
    m_userList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_userList->setTitle("Users");
    m_userList->setAssignList(Ramses::instance()->users());
    tabWidget->addTab(m_userList, QIcon(":/icons/users"), "Users");

    m_applicationList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, tabWidget);
    m_applicationList->setEditMode(ObjectListEditWidget::UnassignObjects);
    m_applicationList->setTitle("Applications");
    m_applicationList->setAssignList(Ramses::instance()->applications());
    tabWidget->addTab(m_applicationList, QIcon(":/icons/applications"), "Applications");

    ui_mainLayout->addWidget(tabWidget);
}

void StepEditWidget::connectEvents()
{
    connect(ui_estimationMultiplierCheckBox, SIGNAL(clicked(bool)), ui_estimationMultiplierBox, SLOT(setEnabled(bool)));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_estimationTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updateEstimationSuffix()));
    connect(ui_typeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(m_userList, SIGNAL(add()), this, SLOT(createUser()));
    connect(m_applicationList, SIGNAL(add()), this, SLOT(createApplication()));
}
