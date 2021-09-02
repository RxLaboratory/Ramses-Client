#include "projectpage.h"

ProjectPage::ProjectPage(QWidget *parent):
    SettingsWidget("Project Administration", parent)
{
    this->showReinitButton(false);

    qDebug() << "> Project";
    // A better layout for project settings
    QWidget *pSettingsWidget = new QWidget(this);
    QHBoxLayout *pSettingsLayout = new QHBoxLayout(pSettingsWidget);
    pSettingsLayout->setContentsMargins(3,3,3,3);
    pSettingsLayout->addStretch();
    ui_currentProjectSettings = new ProjectEditWidget(this);
    pSettingsLayout->addWidget(ui_currentProjectSettings);
    pSettingsLayout->addStretch();
    pSettingsLayout->setStretch(0, 20);
    pSettingsLayout->setStretch(1, 80);
    pSettingsLayout->setStretch(2, 20);
    this->addPage( pSettingsWidget, "Settings", QIcon(":/icons/projects"));

    QMenu *projectMenu = new QMenu();
    QToolButton *projectButton = new QToolButton();
    projectButton->setIcon(QIcon(":icons/project"));
    projectButton->setText(" Project");
    projectButton->setMenu(projectMenu);
    projectButton->setIconSize(QSize(16,16));
    projectButton->setObjectName("menuButton");
    projectButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    projectButton->setPopupMode(QToolButton::InstantPopup);
    this->titleBar()->insertLeft(projectButton);
    RamObjectListMenu *assignUserMenu = new RamObjectListMenu(false, this);
    assignUserMenu->setTitle("Assign user");
    assignUserMenu->setList(Ramses::instance()->users());
    projectMenu->addMenu(assignUserMenu);
    RamObjectListMenu *unAssignUserMenu = new RamObjectListMenu(false, this);
    unAssignUserMenu->setTitle("Unassign user");
    projectMenu->addMenu(unAssignUserMenu);
    // TODO Project (un)assign user

    qDebug() << "  > project settings ok";

    StepListManagerWidget *stepManager = new StepListManagerWidget(this);
    this->addPage(stepManager, "Steps", QIcon(":/icons/steps"));
    this->titleBar()->insertLeft(stepManager->menuButton());

    // Create step from template menu
    RamObjectListMenu *stepTemplateMenu = new RamObjectListMenu(false, this);
    stepTemplateMenu->setTitle("Create from template...");
    stepTemplateMenu->setList( Ramses::instance()->templateSteps() );
    stepManager->menuButton()->menu()->addMenu(stepTemplateMenu);
    // TODO connect "assign" signal to create step

    qDebug() << "  > steps ok";

    PipeFileListManagerWidget *pipeFileManager = new PipeFileListManagerWidget(this);
    this->addPage(pipeFileManager, "Pipe Types", QIcon(":/icons/pipe-files"));
    this->titleBar()->insertLeft(pipeFileManager->menuButton());
    qDebug() << "  > pipe types ok";

    AssetGroupListManagerWidget *assetGroupManager = new AssetGroupListManagerWidget(this);
    this->addPage(assetGroupManager, "Asset Groups", QIcon(":/icons/asset-groups"));
    this->titleBar()->insertLeft(assetGroupManager->menuButton());

    RamObjectListMenu *agTemplateMenu = new RamObjectListMenu(false, this);
    agTemplateMenu->setTitle("Create from template...");
    agTemplateMenu->setList( Ramses::instance()->templateAssetGroups() );
    assetGroupManager->menuButton()->menu()->addMenu(agTemplateMenu);
    // TODO connect "assign" signal to create asset group

    qDebug() << "  > asset groups ok";

    AssetListManagerWidget *assetListManager = new AssetListManagerWidget(this);
    this->addPage(assetListManager, "Assets", QIcon(":/icons/assets"));
    this->titleBar()->insertLeft(assetListManager->menuButton());
    qDebug() << "  > assets ok";

    SequenceListManagerWidget *sequenceManager = new SequenceListManagerWidget(this);
    this->addPage(sequenceManager, "Sequences", QIcon(":/icons/sequences"));
    this->titleBar()->insertLeft(sequenceManager->menuButton());
    qDebug() << "  > sequences ok";

    ShotListManagerWidget *shotManager = new ShotListManagerWidget(this);
    this->addPage(shotManager, "Shots", QIcon(":/icons/shots"));
    this->titleBar()->insertLeft(shotManager->menuButton());
    qDebug() << "  > shots ok";

    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
}

void ProjectPage::currentProjectChanged(RamProject *project)
{
    ui_currentProjectSettings->setObject(project);
}
