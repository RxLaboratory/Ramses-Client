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
    ui_assignUserMenu = new RamObjectListMenu(false, this);
    ui_assignUserMenu->setTitle("Assign user");
    ui_assignUserMenu->setList(Ramses::instance()->users());
    projectMenu->addMenu(ui_assignUserMenu);
    ui_unAssignUserMenu = new RamObjectListMenu(false, this);
    ui_unAssignUserMenu->setTitle("Unassign user");
    projectMenu->addMenu(ui_unAssignUserMenu);

    qDebug() << "  > project settings ok";

    StepListManagerWidget *stepManager = new StepListManagerWidget(this);
    this->addPage(stepManager, "Steps", QIcon(":/icons/steps"));
    this->titleBar()->insertLeft(stepManager->menuButton());

    // Create step from template menu
    RamObjectListMenu *stepTemplateMenu = new RamObjectListMenu(false, this);
    stepTemplateMenu->setTitle("Create from template...");
    stepTemplateMenu->setList( Ramses::instance()->templateSteps() );
    stepManager->menuButton()->menu()->addMenu(stepTemplateMenu);

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

    // Create multiple shots
    QAction *createMultipleShotsAction = new QAction("Create multiple shots...", this);
    shotManager->menuButton()->menu()->addAction(createMultipleShotsAction);

    qDebug() << "  > shots ok";

    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
    connect(ui_assignUserMenu, SIGNAL(assign(RamObject*)), this, SLOT(assignUser(RamObject*)));
    connect(ui_unAssignUserMenu, SIGNAL(assign(RamObject*)), this, SLOT(unAssignUser(RamObject*)));
    connect(stepTemplateMenu, SIGNAL(assign(RamObject*)), this, SLOT(createStepFromTemplate(RamObject*)));
    connect(agTemplateMenu, SIGNAL(assign(RamObject*)), this, SLOT(createAssetGroupFromTemplate(RamObject*)));
    connect(createMultipleShotsAction, SIGNAL(triggered()), this, SLOT(createShots()));
}

void ProjectPage::currentProjectChanged(RamProject *project)
{
    while(!m_userConnections.isEmpty()) disconnect(m_userConnections.takeLast());

    ui_currentProjectSettings->setObject(project);
    if(project)
    {
        ui_unAssignUserMenu->setList(project->users());
        // hide already assigned
        if (project->users()->count())
            userAssigned(QModelIndex(), 0, project->users()->count() - 1);
        m_userConnections << connect(project->users(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(userAssigned(QModelIndex,int,int)));
        m_userConnections << connect(project->users(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(userUnassigned(QModelIndex,int,int)));
    }
    else
        ui_unAssignUserMenu->setList(nullptr);
}

void ProjectPage::assignUser(RamObject *userObj)
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    proj->users()->append( userObj );
}

void ProjectPage::unAssignUser(RamObject *userObj)
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    proj->users()->removeAll(userObj);
}

void ProjectPage::userAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = reinterpret_cast<RamObject*>( proj->users()->data( proj->users()->index(i,0), Qt::UserRole ).toULongLong() );
        ui_assignUserMenu->setObjectVisible(assignedObj, false);
    }
}

void ProjectPage::userUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;

    for (int i = first ; i <= last; i++)
    {
        RamObject *assignedObj = reinterpret_cast<RamObject*>( proj->users()->data( proj->users()->index(i,0), Qt::UserRole ).toULongLong() );
        if (!assignedObj) continue;
        ui_assignUserMenu->setObjectVisible(assignedObj, true);
    }
}

void ProjectPage::createStepFromTemplate(RamObject *stepObj)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamStep *templateStep= qobject_cast<RamStep*>( stepObj );
    if (!templateStep) return;
    RamStep *step = templateStep->createFromTemplate(project);
    project->steps()->append(step);
    step->edit();
}

void ProjectPage::createAssetGroupFromTemplate(RamObject *agObj)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    RamAssetGroup *templateAG = qobject_cast<RamAssetGroup*>( agObj );
    if (!templateAG) return;
    RamAssetGroup *ag = templateAG->createFromTemplate(project);
    project->assetGroups()->append(ag);
    ag->edit();
}

void ProjectPage::createShots()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;

    ShotsCreationDialog dialog(proj, this);
    dialog.exec();
}
