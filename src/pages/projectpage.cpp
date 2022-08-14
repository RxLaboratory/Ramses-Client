#include "projectpage.h"

#include "assetgrouplistmanagerwidget.h"
#include "assetlistmanagerwidget.h"
#include "pipefilelistmanagerwidget.h"
#include "ramassetgroup.h"
#include "ramses.h"
#include "sequencelistmanagerwidget.h"
#include "shotlistmanagerwidget.h"
#include "shotscreationdialog.h"
#include "steplistmanagerwidget.h"

ProjectPage::ProjectPage(QWidget *parent):
    SettingsWidget("Project Administration", parent)
{
    this->showReinitButton(false);
    this->titleBar()->setObjectName("projectToolBar");

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
    ui_assignUserMenu = new RamObjectListMenu<RamUser*>(false, this);
    ui_assignUserMenu->setTitle("Assign user");
    ui_assignUserMenu->setList(Ramses::instance()->users());
    projectMenu->addMenu(ui_assignUserMenu);
    ui_unAssignUserMenu = new RamObjectListMenu<RamUser*>(false, this);
    ui_unAssignUserMenu->setTitle("Unassign user");
    projectMenu->addMenu(ui_unAssignUserMenu);

    qDebug() << "  > project settings ok";

    StepListManagerWidget *stepManager = new StepListManagerWidget(this);
    this->addPage(stepManager, "Steps", QIcon(":/icons/steps"));
    this->titleBar()->insertLeft(stepManager->menuButton());

    // Create step from template menu
    RamObjectListMenu<RamTemplateStep*> *stepTemplateMenu = new RamObjectListMenu<RamTemplateStep*>(false, this);
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

    RamObjectListMenu<RamTemplateAssetGroup*> *agTemplateMenu = new RamObjectListMenu<RamTemplateAssetGroup*>(false, this);
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
    connect(ui_assignUserMenu, SIGNAL(assign(RamUser*)), this, SLOT(assignUser(RamUser*)));
    connect(ui_unAssignUserMenu, SIGNAL(assign(RamUser*)), this, SLOT(unAssignUser(RamUser*)));
    connect(stepTemplateMenu, SIGNAL(assign(RamTemplateStep*)), this, SLOT(createStepFromTemplate(RamTemplateStep*)));
    connect(agTemplateMenu, SIGNAL(assign(RamTemplateAssetGroup*)), this, SLOT(createAssetGroupFromTemplate(RamTemplateAssetGroup*)));
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
        if (project->users()->rowCount())
            userAssigned(QModelIndex(), 0, project->users()->rowCount() - 1);
        m_userConnections << connect(project->users(), SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(userAssigned(QModelIndex,int,int)));
        m_userConnections << connect(project->users(), SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(userUnassigned(QModelIndex,int,int)));
    }
    else
        ui_unAssignUserMenu->setList(nullptr);
}

void ProjectPage::assignUser(RamUser *user)
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    proj->users()->append( user );
}

void ProjectPage::unAssignUser(RamUser *user)
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    proj->users()->removeAll(user);
}

void ProjectPage::userAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;

    for (int i = first ; i <= last; i++)
    {
        RamUser *user = proj->users()->at(i);
        ui_assignUserMenu->setObjectVisible(user, false);
    }
}

void ProjectPage::userUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;

    for (int i = first ; i <= last; i++)
    {
        RamUser *user = proj->users()->at(i);
        ui_assignUserMenu->setObjectVisible(user, true);
    }
}

void ProjectPage::createStepFromTemplate(RamTemplateStep *templateStep)
{
    if (!templateStep) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamStep *step = RamStep::createFromTemplate(templateStep, project);
    project->steps()->append(step);
    step->edit();
}

void ProjectPage::createAssetGroupFromTemplate(RamTemplateAssetGroup *templateAG)
{
    if (!templateAG) return;

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamAssetGroup *ag = RamAssetGroup::createFromTemplate(templateAG, project);
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
