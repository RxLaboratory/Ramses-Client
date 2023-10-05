#include "mainwindow.h"

#include "assetgroupmanagerwidget.h"
#include "assetmanagerwidget.h"
#include "duqf-app/dusettingsmanager.h"
#include "itemmanagerwidget.h"
#include "pipefilemanagerwidget.h"
#include "progressmanager.h"
#include "progressbar.h"
#include "docks/consolewidget.h"
#include "daemonsettingswidget.h"
#include "loginpage.h"
#include "sequencemanagerwidget.h"
#include "shotmanagerwidget.h"
#include "stepmanagerwidget.h"
#include "userprofilepage.h"
#include "usermanagerwidget.h"
#include "projectmanagerwidget.h"
#include "pipeline-editor/pipelinewidget.h"
#include "templatestepmanagerwidget.h"
#include "templateassetgroupmanagerwidget.h"
#include "statemanagerwidget.h"
#include "filetypemanagerwidget.h"
#include "applicationmanagerwidget.h"
#include "schedulemanagerwidget.h"
#include "docks/statisticswidget.h"
#include "docks/timelinewidget.h"
#include "dbinterface.h"
#include "daemon.h"
#include "projectselectorwidget.h"
#include "dbmanagerwidget.h"
#include "duqf-widgets/dutoolbarspacer.h"
#include "duqf-widgets/duqflogtoolbutton.h"
#include "duqf-widgets/duqfupdatesettingswidget.h"
#include "duqf-widgets/appearancesettingswidget.h"
#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-app/app-utils.h"
#include "duqf-widgets/dutoolbarspacer.h"

MainWindow::MainWindow(QStringList /*args*/, QWidget *parent) :
    QMainWindow(parent)
{
    // Build the form
    setupUi(this);

    qDebug() << "> Initiating threads and workers";

    // We instantiate all these objects to be sure it's done in the right order.

    // The database interface
    DBInterface::instance();
    // The Process manager
    ProgressManager::instance();
    // Ramses
    Ramses::instance();

    qDebug() << "> Loading settings";

    QSettings settings;

    qDebug() << "> Setting up menus";

    // Setup toolbar menus
    QMenu *pipelineMenu = new QMenu(this);

    ui_projectAction = new QAction(tr("Project settings"), this);
    ui_projectAction->setIcon(QIcon(":/icons/project"));
    ui_projectAction->setCheckable(true);
    ui_stepsAction = new QAction(tr("Steps"), this);
    ui_stepsAction->setIcon(QIcon(":/icons/step"));
    ui_stepsAction->setCheckable(true);
    ui_pipeFilesAction = new QAction(tr("Pipe formats"), this);
    ui_pipeFilesAction->setIcon(QIcon(":/icons/connection"));
    ui_pipeFilesAction->setCheckable(true);

    pipelineMenu->addAction(ui_actionPipeline);
    pipelineMenu->addSeparator();
    pipelineMenu->addAction(ui_projectAction);
    pipelineMenu->addAction(ui_stepsAction);
    pipelineMenu->addAction(ui_pipeFilesAction);

    ui_pipelineButton = new QToolButton();
    ui_pipelineButton->setIcon(QIcon(":icons/steps-menu"));
    ui_pipelineButton->setText("Pipeline");
    ui_pipelineButton->setMenu(pipelineMenu);
    ui_pipelineButton->setIconSize(QSize(16,16));
    ui_pipelineButton->setPopupMode(QToolButton::MenuButtonPopup);
    ui_pipelineButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QMenu *assetsMenu = new QMenu(this);

    ui_assetGroupAction = new QAction(tr("Asset groups"), this);
    ui_assetGroupAction->setIcon(QIcon(":/icons/asset-group"));
    ui_assetGroupAction->setCheckable(true);
    ui_assetListAction = new QAction(tr("Assets"), this);
    ui_assetListAction->setIcon(QIcon(":/icons/asset"));
    ui_assetListAction->setCheckable(true);

    assetsMenu->addAction(ui_actionAssets);
    assetsMenu->addSeparator();
    assetsMenu->addAction(ui_assetGroupAction);
    assetsMenu->addAction(ui_assetListAction);

    ui_assetsButton = new QToolButton();
    ui_assetsButton->setIcon(QIcon(":/icons/asset"));
    ui_assetsButton->setText("Assets");
    ui_assetsButton->setMenu(assetsMenu);
    ui_assetsButton->setIconSize(QSize(16,16));
    ui_assetsButton->setPopupMode(QToolButton::MenuButtonPopup);
    ui_assetsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QMenu *shotsMenu = new QMenu(this);

    ui_sequenceAction = new QAction(tr("Sequences"), this);
    ui_sequenceAction->setIcon(QIcon(":/icons/sequence"));
    ui_sequenceAction->setCheckable(true);
    ui_shotListAction = new QAction(tr("Shots"), this);
    ui_shotListAction->setIcon(QIcon(":/icons/shot"));
    ui_shotListAction->setCheckable(true);

    shotsMenu->addAction(ui_actionShots);
    shotsMenu->addSeparator();
    shotsMenu->addAction(ui_sequenceAction);
    shotsMenu->addAction(ui_shotListAction);
    shotsMenu->addAction(ui_actionTimeline);

    ui_shotsButton = new QToolButton();
    ui_shotsButton->setIcon(QIcon(":/icons/shot"));
    ui_shotsButton->setText("Shots");
    ui_shotsButton->setMenu(shotsMenu);
    ui_shotsButton->setIconSize(QSize(16,16));
    ui_shotsButton->setPopupMode(QToolButton::MenuButtonPopup);
    ui_shotsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QMenu *scheduleMenu = new QMenu(this);

    scheduleMenu->addAction(ui_actionSchedule);
    scheduleMenu->addSeparator();
    scheduleMenu->addAction(ui_actionStatistics);

    ui_scheduleButton = new QToolButton();
    ui_scheduleButton->setIcon(QIcon(":/icons/calendar"));
    ui_scheduleButton->setText("Schedule");
    ui_scheduleButton->setMenu(scheduleMenu);
    ui_scheduleButton->setIconSize(QSize(16,16));
    ui_scheduleButton->setPopupMode(QToolButton::MenuButtonPopup);
    ui_scheduleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    QMenu *filesMenu = new QMenu(this);

    ui_fileAdminAction = new QAction(tr("Admin"), this);
    ui_fileAdminAction->setIcon(QIcon(":/icons/settings-w"));
    ui_filePreprodAction = new QAction(tr("Pre-production"), this);
    ui_filePreprodAction->setIcon(QIcon(":/icons/project"));
    ui_fileProdAction = new QAction(tr("Production"), this);
    ui_fileProdAction->setIcon(QIcon(":/icons/sequence"));
    ui_filePostProdAction = new QAction(tr("Post-production"), this);
    ui_filePostProdAction->setIcon(QIcon(":/icons/film"));
    ui_fileAssetsAction = new QAction(tr("Assets"), this);
    ui_fileAssetsAction->setIcon(QIcon(":/icons/asset"));
    ui_fileShotsAction = new QAction(tr("Shots"), this);
    ui_fileShotsAction->setIcon(QIcon(":/icons/shot"));
    ui_fileOutputAction = new QAction(tr("Output"), this);
    ui_fileOutputAction->setIcon(QIcon(":/icons/output-folder"));
    ui_fileUserAction = new QAction(tr("My user folder"), this);
    ui_fileUserAction->setIcon(QIcon(":/icons/user"));
    ui_fileVersionsAction = new QAction(tr("Project versions"), this);
    ui_fileVersionsAction->setIcon(QIcon(":/icons/versions-folder"));
    ui_fileTrashAction = new QAction(tr("Project trash"), this);
    ui_fileTrashAction->setIcon(QIcon(":/icons/trash"));

    filesMenu->addAction(ui_fileAdminAction);
    filesMenu->addAction(ui_filePreprodAction);
    filesMenu->addAction(ui_fileProdAction);
    filesMenu->addAction(ui_filePostProdAction);
    filesMenu->addAction(ui_fileAssetsAction);
    filesMenu->addAction(ui_fileShotsAction);
    filesMenu->addAction(ui_fileOutputAction);
    filesMenu->addSeparator();
    filesMenu->addAction(ui_fileUserAction);
    filesMenu->addSeparator();
    filesMenu->addAction(ui_fileVersionsAction);
    filesMenu->addAction(ui_fileTrashAction);

    ui_filesButton = new QToolButton();
    ui_filesButton->setIcon(QIcon(":/icons/folder"));
    ui_filesButton->setText("Files");
    ui_filesButton->setMenu(filesMenu);
    ui_filesButton->setIconSize(QSize(16,16));
    ui_filesButton->setPopupMode(QToolButton::InstantPopup);
    ui_filesButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Populate Toolbar
    ui_projectSelectorAction = mainToolBar->addWidget(new ProjectSelectorWidget(this));
    ui_pipelineMenuAction = mainToolBar->addWidget(ui_pipelineButton);
    ui_assetMenuAction = mainToolBar->addWidget(ui_assetsButton);
    ui_shotMenuAction = mainToolBar->addWidget(ui_shotsButton);
    ui_scheduleMenuAction = mainToolBar->addWidget(ui_scheduleButton);
    ui_filesMenuAction = mainToolBar->addWidget(ui_filesButton);

    ui_projectSelectorAction->setVisible(false);
    actionAdmin->setVisible(false);
    ui_pipelineMenuAction->setVisible(false);
    ui_shotMenuAction->setVisible(false);
    ui_assetMenuAction->setVisible(false);
    ui_scheduleMenuAction->setVisible(false);
    ui_filesMenuAction->setVisible(false);

    //Populate status bar

    mainStatusBar->addPermanentWidget(new ProgressBar(this));

    ui_refreshMenu = new QMenu();
    ui_refreshMenu->addAction(actionSync);
    ui_refreshMenu->addAction(actionFullSync);

    ui_refreshButton = new QToolButton(this);
    ui_refreshButton->setObjectName("menuButton");
    ui_refreshButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui_refreshButton->setText("");
    ui_refreshButton->setIcon(QIcon(":/icons/reload"));
    ui_refreshButton->setMenu(ui_refreshMenu);
    ui_refreshButton->setPopupMode(QToolButton::InstantPopup);
    mainStatusBar->addPermanentWidget(ui_refreshButton);
    ui_refreshButton->hide();

    ui_consoleButton = new QToolButton(this);
    ui_consoleButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui_consoleButton->setText("");
    ui_consoleButton->setIcon(QIcon(":/icons/bash"));
    ui_consoleButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(ui_consoleButton);

    mainStatusBar->addPermanentWidget(new DuQFLogToolButton(this));

    ui_databaseMenu = new QMenu();
    ui_databaseMenu->addAction(actionSetOffline);
    ui_databaseMenu->addAction(actionSetOnline);
    ui_databaseMenu->addAction(actionDatabaseSettings);
    actionSetOffline->setVisible(false);
    ui_networkButton = new DuQFAutoSizeToolButton(this);
    ui_networkButton->setObjectName("menuButton");
    ui_networkButton->setText("Offline");
    ui_networkButton->setIcon(QIcon(":/icons/storage"));
    ui_networkButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    //ui_networkButton->setMinimumWidth(100);
    ui_networkButton->setMenu(ui_databaseMenu);
    ui_networkButton->setPopupMode(QToolButton::InstantPopup);
    mainStatusBar->addPermanentWidget(ui_networkButton);
    ui_networkButton->setVisible(false);

    ui_userMenu = new QMenu();
    ui_userMenu->addAction(actionLogIn);
    ui_userMenu->addAction(actionUserFolder);
    actionUserFolder->setVisible(false);
    ui_userMenu->addAction(actionUserProfile);
    actionUserProfile->setVisible(false);
    ui_userMenu->addAction(actionLogOut);
    actionLogOut->setVisible(false);
    ui_userButton = new DuQFAutoSizeToolButton(this);
    ui_userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_userButton->setText("Guest");
    //ui_userButton->setMinimumWidth(75);
    ui_userButton->setMenu(ui_userMenu);
    ui_userButton->setPopupMode(QToolButton::InstantPopup);
    mainStatusBar->addPermanentWidget(ui_userButton);

    // Add default stuff
    duqf_initUi();

    qDebug() << "> Loading setting pages";

    // Add settings
    DaemonSettingsWidget *dsw = new DaemonSettingsWidget(this);
    settingsWidget->addPage(dsw, "Daemon", QIcon(":/icons/daemon"));

    qDebug() << "> Loading pages";

    // Add pages
    // login
    LoginPage *lp = new LoginPage(this);
    mainLayout->addWidget(lp);

    // user profile
    UserProfilePage *up = new UserProfilePage(this);
    mainStack->addWidget(up);

    // admin
    ui_adminPage = new SettingsWidget("Administration", this);
    ui_adminPage->titleBar()->setObjectName("adminToolBar");
    ui_adminPage->showReinitButton(false);
    mainStack->addWidget(ui_adminPage);
    // Admin tabs
    qDebug() << "> Admin";
    UserManagerWidget *userManager = new UserManagerWidget(this);
    ui_adminPage->addPage(userManager,"Users", QIcon(":/icons/users"));
    ui_adminPage->titleBar()->insertLeft(userManager->menuButton());
    qDebug() << "  > users ok";
    ProjectManagerWidget *projectManager = new ProjectManagerWidget(this);
    ui_adminPage->addPage(projectManager, "Projects", QIcon(":/icons/projects"));
    ui_adminPage->titleBar()->insertLeft(projectManager->menuButton());
    qDebug() << "  > projects ok";
    TemplateStepManagerWidget *templateStepManager = new TemplateStepManagerWidget(this);
    ui_adminPage->addPage(templateStepManager, "Template Steps", QIcon(":/icons/steps"));
    ui_adminPage->titleBar()->insertLeft(templateStepManager->menuButton());
    qDebug() << "  > template steps ok";
    TemplateAssetGroupManagerWidget *templateAssetGroupManager = new TemplateAssetGroupManagerWidget(this);
    ui_adminPage->addPage(templateAssetGroupManager, "Template Asset Groups", QIcon(":/icons/asset-groups"));
    ui_adminPage->titleBar()->insertLeft(templateAssetGroupManager->menuButton());
    qDebug() << "  > template assets ok";
    StateManagerWidget *stateManager = new StateManagerWidget(this);
    ui_adminPage->addPage(stateManager, "States", QIcon(":/icons/state"));
    ui_adminPage->titleBar()->insertLeft(stateManager->menuButton());
    qDebug() << "  > states ok";
    FileTypeManagerWidget *fileTypeManager = new FileTypeManagerWidget(this);
    ui_adminPage->addPage(fileTypeManager, "File Types", QIcon(":/icons/files"));
    ui_adminPage->titleBar()->insertLeft(fileTypeManager->menuButton());
    qDebug() << "  > file types ok";
    ApplicationManagerWidget *applicationManager = new ApplicationManagerWidget(this);
    ui_adminPage->addPage(applicationManager, "Applications", QIcon(":/icons/applications"));
    ui_adminPage->titleBar()->insertLeft(applicationManager->menuButton());
    qDebug() << "  > applications ok";
    DBManagerWidget *dbManager = new DBManagerWidget(this);
    ui_adminPage->addPage(dbManager, "Database tools", QIcon(":/icons/applications"));
    qDebug() << "  > DB Manager ok";//*/

    // Pipeline editor
#ifndef DEACTIVATE_PIPELINE
    PipelineWidget *pipelineEditor = new PipelineWidget(this);
    mainStack->addWidget(pipelineEditor);
    connect(pipelineEditor, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Pipeline ready";//*/
#endif

#ifndef DEACTIVATE_ASSETSTABLE
    ItemManagerWidget *assetsTable = new ItemManagerWidget(RamStep::AssetProduction, this);
    mainStack->addWidget(assetsTable);
    connect(assetsTable, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Assets table ready";
#endif

#ifndef DEACTIVATE_SHOTSTABLE
    ItemManagerWidget *shotsTable = new ItemManagerWidget(RamStep::ShotProduction, this);
    mainStack->addWidget(shotsTable);
    connect(shotsTable, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Shots table ready";
#endif

#ifndef DEACTIVATE_SCHEDULE
    ScheduleManagerWidget *scheduleTable = new ScheduleManagerWidget(this);
    mainStack->addWidget(scheduleTable);
    connect(scheduleTable,SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Schedule ready";
#endif

    // Docks
#ifndef DEACTIVATE_STATS
    StatisticsWidget *statsTable = new StatisticsWidget(this);
    ui_statsDockWidget = new QDockWidget("Statistics");
    ui_statsDockWidget->setObjectName("statsDock");
    ui_statsTitle = new DuQFDockTitle("Statistics", this);
    ui_statsTitle->setObjectName("dockTitle");
    ui_statsTitle->setIcon(":/icons/stats");
    ui_statsDockWidget->setTitleBarWidget(ui_statsTitle);
    ui_statsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_statsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_statsDockWidget->setWidget( statsTable );
    Qt::DockWidgetArea area = static_cast<Qt::DockWidgetArea>( settings.value("ui/statsArea", Qt::LeftDockWidgetArea).toInt() );
    this->addDockWidget(area, ui_statsDockWidget);
    ui_statsDockWidget->hide();

    qDebug() << "> Statistics table ready";
#endif

    // A console in a tab
    ConsoleWidget *console = new ConsoleWidget(this);
    ui_consoleDockWidget = new QDockWidget("Console");
    ui_consoleDockWidget->setObjectName("consoleDock");
    DuQFDockTitle *consoleTitle = new DuQFDockTitle("Console", this);
    consoleTitle->setObjectName("dockTitle");
    consoleTitle->setIcon(":/icons/bash");
    ui_consoleDockWidget->setTitleBarWidget(consoleTitle);
    ui_consoleDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_consoleDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_consoleDockWidget->setWidget( console );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_consoleDockWidget);
    ui_consoleDockWidget->hide();

    qDebug() << "> Console dock ready";

    // The timeline
    TimelineWidget *timeline = new TimelineWidget(this);
    ui_timelineDockWidget = new QDockWidget("Timeline");
    ui_timelineDockWidget->setObjectName("timelineDock");
    DuQFDockTitle *timelineTitle = new DuQFDockTitle("Timeline", this);
    timeline->setObjectName("dockTitle");
    timelineTitle->setIcon(":/icons/timeline");
    ui_timelineDockWidget->setTitleBarWidget(timelineTitle);
    ui_timelineDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    ui_timelineDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_timelineDockWidget->setWidget( timeline );
    this->addDockWidget(Qt::BottomDockWidgetArea, ui_timelineDockWidget);
    ui_timelineDockWidget->hide();

    qDebug() << "> Timeline dock ready";

    // The properties dock
    ui_propertiesDockWidget = new QDockWidget("Properties");
    ui_propertiesDockWidget->setObjectName("propertiesDock");
    ui_propertiesTitle = new DuQFDockTitle("Properties", this);
    ui_propertiesTitle->setObjectName("dockTitle");
    ui_propertiesTitle->setIcon(":/icons/asset");
    ui_propertiesDockWidget->setTitleBarWidget(ui_propertiesTitle);
    ui_propertiesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_propertiesDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::RightDockWidgetArea, ui_propertiesDockWidget);
    ui_propertiesDockWidget->hide();

    qDebug() << "> Properties dock ready";

    // The project dock
    ui_projectEditWiget = new ProjectEditWidget(this);
    ui_projectDockWidget = new QDockWidget(tr("Project settings"));
    ui_projectDockWidget->setObjectName("projectDock");
    DuQFDockTitle *pTitle = new DuQFDockTitle(tr("Project settings"), this);
    pTitle->setObjectName("dockTitle");
    pTitle->setIcon(":/icons/project");
    ui_projectDockWidget->setTitleBarWidget(pTitle);
    ui_projectDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_projectDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_projectDockWidget->setWidget( ui_projectEditWiget );
    this->addDockWidget(Qt::RightDockWidgetArea, ui_projectDockWidget);
    ui_projectDockWidget->hide();

    // The steps dock
    StepManagerWidget *stepWidget = new StepManagerWidget(this);
    ui_stepsDockWidget = new QDockWidget(tr("Steps"));
    ui_stepsDockWidget->setObjectName("stepsDock");
    DuQFDockTitle *sTitle = new DuQFDockTitle(tr("Steps"), this);
    sTitle->setObjectName("dockTitle");
    sTitle->setIcon(":/icons/step");
    ui_stepsDockWidget->setTitleBarWidget(sTitle);
    ui_stepsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_stepsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_stepsDockWidget->setWidget( stepWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_stepsDockWidget);
    ui_stepsDockWidget->hide();

    // The pipe formats dock
    PipeFileManagerWidget *pipeFileWidget = new PipeFileManagerWidget(this);
    ui_pipeFileDockWidget = new QDockWidget(tr("Pipe formats"));
    ui_pipeFileDockWidget->setObjectName("pipeFilesDock");
    DuQFDockTitle *pfTitle = new DuQFDockTitle(tr("Pipe formats"), this);
    pfTitle->setObjectName("dockTitle");
    pfTitle->setIcon(":/icons/connection");
    ui_pipeFileDockWidget->setTitleBarWidget(pfTitle);
    ui_pipeFileDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_pipeFileDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_pipeFileDockWidget->setWidget( pipeFileWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_pipeFileDockWidget);
    ui_pipeFileDockWidget->hide();

    // The asset groups dock
    AssetGroupManagerWidget *assetGroupWidget = new AssetGroupManagerWidget(this);
    ui_assetGroupsDockWidget = new QDockWidget(tr("Asset Groups"));
    ui_assetGroupsDockWidget->setObjectName("assetGroupsDock");
    DuQFDockTitle *agTitle = new DuQFDockTitle(tr("Asset Groups"), this);
    agTitle->setObjectName("dockTitle");
    agTitle->setIcon(":/icons/asset-group");
    ui_assetGroupsDockWidget->setTitleBarWidget(agTitle);
    ui_assetGroupsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_assetGroupsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_assetGroupsDockWidget->setWidget( assetGroupWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_assetGroupsDockWidget);
    ui_assetGroupsDockWidget->hide();

    // The assets dock
    AssetManagerWidget *assetWidget = new AssetManagerWidget(this);
    ui_assetsDockWidget = new QDockWidget(tr("Assets"));
    ui_assetsDockWidget->setObjectName("assetsDock");
    DuQFDockTitle *aTitle = new DuQFDockTitle(tr("Assets"), this);
    aTitle->setObjectName("dockTitle");
    aTitle->setIcon(":/icons/asset");
    ui_assetsDockWidget->setTitleBarWidget(aTitle);
    ui_assetsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_assetsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_assetsDockWidget->setWidget( assetWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_assetsDockWidget);
    ui_assetsDockWidget->hide();

    // The sequences dock
    SequenceManagerWidget *sequenceWidget = new SequenceManagerWidget(this);
    ui_sequencesDockWidget = new QDockWidget(tr("Sequences"));
    ui_sequencesDockWidget->setObjectName("sequencesDock");
    DuQFDockTitle *seqTitle = new DuQFDockTitle(tr("Sequences"), this);
    seqTitle->setObjectName("dockTitle");
    seqTitle->setIcon(":/icons/sequence");
    ui_sequencesDockWidget->setTitleBarWidget(seqTitle);
    ui_sequencesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_sequencesDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_sequencesDockWidget->setWidget( sequenceWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_sequencesDockWidget);
    ui_sequencesDockWidget->hide();

    // The Shots dock
    ShotManagerWidget *shotWidget = new ShotManagerWidget(this);
    ui_shotsDockWidget = new QDockWidget(tr("Shots"));
    ui_shotsDockWidget->setObjectName("shotsDock");
    DuQFDockTitle *shTitle = new DuQFDockTitle(tr("Shots"), this);
    shTitle->setObjectName("dockTitle");
    shTitle->setIcon(":/icons/shot");
    ui_shotsDockWidget->setTitleBarWidget(shTitle);
    ui_shotsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_shotsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_shotsDockWidget->setWidget( shotWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_shotsDockWidget);
    ui_shotsDockWidget->hide();

    // Progress page
    progressPage = new ProgressPage(this);
    mainStack->addWidget(progressPage);

    // Set UI
    mainStack->setCurrentIndex(Home);

    qDebug() << "> Connecting events";

    connectEvents();
    connectShortCuts();

    // Set style

    qDebug() << "Setting CSS";

    duqf_setStyle();

    // Restore UI state
    settings.beginGroup("ui");
    if (settings.value("maximized", false).toBool() )
        this->showMaximized();
    this->restoreState( settings.value("windowState").toByteArray() );
    settings.endGroup();
    // re-hide docks
    ui_propertiesDockWidget->hide();
    //ui_statsDockWidget->hide();
    //ui_timelineDockWidget->hide();

    // Process arguments

    // Load file
    DuApplication *app = qobject_cast<DuApplication*>(qApp);
    QStringList args = app->args();
    if (args.count() != 1) return;

    QString filePath = args.first();
    QFileInfo argInfo(filePath);
    if (argInfo.exists() && argInfo.suffix().toLower() == "ramses")
        StateManager::i()->open(filePath);
}

void MainWindow::connectEvents()
{
    // Connect events
    connect(ProgressManager::instance(), &ProgressManager::freezeUI, this, &MainWindow::freezeUI);

    // Toolbar buttons
    connect(actionLogIn,SIGNAL(triggered()), this, SLOT(home()));
    connect(actionLogOut,SIGNAL(triggered()), StateManager::i(), SLOT(logout()));
    connect(actionSettings, SIGNAL(triggered(bool)), this, SLOT(duqf_settings(bool)));
    connect(actionSetOnline, &QAction::triggered, this, &MainWindow::setOnlineAction);
    connect(actionSetOffline, &QAction::triggered, this, &MainWindow::setOfflineAction);
    connect(actionDatabaseSettings, &QAction::triggered, this, &MainWindow::databaseSettingsAction);
    connect(actionUserProfile,SIGNAL(triggered()), this, SLOT(userProfile()));
    connect(actionUserFolder,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(actionAdmin,SIGNAL(triggered(bool)), this, SLOT(admin(bool)));

    connect(ui_actionPipeline,SIGNAL(triggered()), this, SLOT(pipeline()));
    connect(ui_pipelineButton,SIGNAL(clicked()), this, SLOT(pipeline()));

    connect(ui_actionShots,SIGNAL(triggered(bool)), this, SLOT(shots()));
    connect(ui_shotsButton,SIGNAL(clicked()), this, SLOT(shots()));

    connect(ui_actionAssets,SIGNAL(triggered(bool)), this, SLOT(assets()));
    connect(ui_assetsButton,SIGNAL(clicked()), this, SLOT(assets()));

    connect(ui_actionSchedule,SIGNAL(triggered(bool)), this, SLOT(schedule()));
    connect(ui_scheduleButton, SIGNAL(clicked()), this, SLOT(schedule()));

    // Docks
    connect(ui_projectAction, SIGNAL(triggered(bool)), ui_projectDockWidget, SLOT(setVisible(bool)));
    connect(ui_projectDockWidget, SIGNAL(visibilityChanged(bool)), ui_projectAction, SLOT(setChecked(bool)));

    connect(ui_stepsAction, SIGNAL(triggered(bool)), ui_stepsDockWidget, SLOT(setVisible(bool)));
    connect(ui_stepsDockWidget, SIGNAL(visibilityChanged(bool)), ui_stepsAction, SLOT(setChecked(bool)));

    connect(ui_pipeFilesAction, SIGNAL(triggered(bool)), ui_pipeFileDockWidget, SLOT(setVisible(bool)));
    connect(ui_pipeFileDockWidget, SIGNAL(visibilityChanged(bool)), ui_pipeFilesAction, SLOT(setChecked(bool)));

    connect(ui_assetGroupAction, SIGNAL(triggered(bool)), ui_assetGroupsDockWidget, SLOT(setVisible(bool)));
    connect(ui_assetGroupsDockWidget, SIGNAL(visibilityChanged(bool)), ui_assetGroupAction, SLOT(setChecked(bool)));

    connect(ui_assetListAction, SIGNAL(triggered(bool)), ui_assetsDockWidget, SLOT(setVisible(bool)));
    connect(ui_assetsDockWidget, SIGNAL(visibilityChanged(bool)), ui_assetListAction, SLOT(setChecked(bool)));

    connect(ui_sequenceAction, SIGNAL(triggered(bool)), ui_sequencesDockWidget, SLOT(setVisible(bool)));
    connect(ui_sequencesDockWidget, SIGNAL(visibilityChanged(bool)), ui_sequenceAction, SLOT(setChecked(bool)));

    connect(ui_shotListAction, SIGNAL(triggered(bool)), ui_shotsDockWidget, SLOT(setVisible(bool)));
    connect(ui_shotsDockWidget, SIGNAL(visibilityChanged(bool)), ui_shotListAction, SLOT(setChecked(bool)));

    connect(ui_actionStatistics,SIGNAL(triggered(bool)), ui_statsDockWidget, SLOT(setVisible(bool)));
    connect(ui_statsDockWidget,SIGNAL(visibilityChanged(bool)), ui_actionStatistics, SLOT(setChecked(bool)));

    connect(ui_consoleDockWidget,SIGNAL(visibilityChanged(bool)), ui_consoleButton, SLOT(setChecked(bool)));
    connect(ui_consoleButton,SIGNAL(clicked(bool)), ui_consoleDockWidget, SLOT(setVisible(bool)));

    connect(ui_actionTimeline,SIGNAL(triggered(bool)), ui_timelineDockWidget, SLOT(setVisible(bool)));
    connect(ui_timelineDockWidget,SIGNAL(visibilityChanged(bool)), ui_actionTimeline, SLOT(setChecked(bool)));

    // Files
    connect(ui_fileAdminAction,SIGNAL(triggered()), this, SLOT(revealAdminFolder()));
    connect(ui_filePreprodAction,SIGNAL(triggered()), this, SLOT(revealPreProdFolder()));
    connect(ui_fileProdAction,SIGNAL(triggered()), this, SLOT(revealProdFolder()));
    connect(ui_filePostProdAction,SIGNAL(triggered()), this, SLOT(revealPostProdFolder()));
    connect(ui_fileAssetsAction,SIGNAL(triggered()), this, SLOT(revealAssetsFolder()));
    connect(ui_fileShotsAction,SIGNAL(triggered()), this, SLOT(revealShotsFolder()));
    connect(ui_fileOutputAction,SIGNAL(triggered()), this, SLOT(revealOutputFolder()));
    connect(ui_fileUserAction,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(ui_fileVersionsAction,SIGNAL(triggered()), this, SLOT(revealVersionsFolder()));
    connect(ui_fileTrashAction,SIGNAL(triggered()), this, SLOT(revealTrashFolder()));

    // Pages
    connect(ui_adminPage, SIGNAL(closeRequested()), this, SLOT(home()));

    // Other buttons
    connect(actionSync, SIGNAL(triggered()), DBInterface::instance(),SLOT(sync()));
    connect(actionFullSync, SIGNAL(triggered()), DBInterface::instance(),SLOT(fullSync()));
    connect(mainStack,SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));

    // Misc
    connect(DuQFLogger::instance(), &DuQFLogger::newLog, this, &MainWindow::log);
    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::duqf_raise);
    connect(Ramses::instance(),&Ramses::userChanged, this, &MainWindow::currentUserChanged);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &MainWindow::currentProjectChanged);
    connect(DBInterface::instance(),&DBInterface::connectionStatusChanged, this, &MainWindow::dbiConnectionStatusChanged);
    connect(DBInterface::instance(), &DBInterface::syncFinished, this, &MainWindow::finishSync);
    connect(DBInterface::instance(), &DBInterface::syncStarted, this, &MainWindow::startSync);
}

void MainWindow::connectShortCuts()
{
    QList<QKeySequence> syncSC;
    syncSC << QKeySequence("Ctrl+R");
    syncSC << QKeySequence("F5");
    actionSync->setShortcuts( syncSC );
    actionFullSync->setShortcut(QKeySequence("Ctrl+Shift+R"));
}

void MainWindow::setPropertiesDockWidget(QWidget *w, QString title, QString icon)
{
    ui_propertiesDockWidget->setWidget( w );
    ui_propertiesTitle->setTitle(title);
    ui_propertiesTitle->setIcon(icon);
    ui_propertiesDockWidget->show();
}

void MainWindow::setMaximizedState(bool maximized)
{
    m_maximizeAction->setChecked(maximized);
    m_maximized = maximized;
}

void MainWindow::hidePropertiesDock()
{
    ui_propertiesDockWidget->hide();
}

void MainWindow::onQuit()
{
    QFontDatabase::removeAllApplicationFonts();
    trayIcon->hide();
}

void MainWindow::duqf_initUi()
{
    // Set transparent, and draw the background in paintEvent() to have rounded corners
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setStyleSheet("#centralWidget { background: none; }"
                  "#mainToolBar { border-radius: 10px; margin: 5px 5px 5px 5px; }");

    // ===== SYSTRAY ======
    duqf_actionShowHide = new QAction("Hide " + QString(STR_INTERNALNAME), this);
    duqf_actionShowHide->setIcon(QIcon(":/icons/hide-dark"));
    bool useSysTray = QSystemTrayIcon::isSystemTrayAvailable() && USE_SYSTRAY;
    if (useSysTray)
    {
        QMenu *trayMenu = new QMenu(QString(STR_INTERNALNAME),this);
        QSettings settings;
#ifdef Q_OS_LINUX
        QString trayIconType = settings.value("appearance/trayIconType", "light").toString();
#else
        QString trayIconType = settings.value("appearance/trayIconType", "color").toString();
#endif
        trayIcon = new QSystemTrayIcon(QIcon(":/icons/tray-" + trayIconType),this);
        trayMenu->addAction(duqf_actionShowHide);
        QAction *actionQuit = new QAction("Quit");
        actionQuit->setIcon(QIcon(":/icons/close-dark"));
        trayMenu->addAction(actionQuit);
        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();
        connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(duqf_trayClicked(QSystemTrayIcon::ActivationReason)));
        connect(duqf_actionShowHide, &QAction::triggered, this, &MainWindow::duqf_showHide);
        connect(actionQuit, &QAction::triggered, this, &MainWindow::close);
    }

    // ===== ABOUT ========
    duqf_aboutDialog = new AboutDialog();

    // ===== TOOLBAR ======

    mainToolBar->setWindowTitle(QString(STR_FILEDESCRIPTION));
    QString focusColor = DuSettingsManager::instance()->uiFocusColor(DuSettingsManager::DarkerColor).name();
    mainToolBar->setStyleSheet("#windowButton:hover,"
                               "QToolButton:hover"
                               "{ background-color:" + focusColor + "}");

    // remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);

    //drag window
    duqf_toolBarClicked = false;

#ifdef Q_OS_LINUX
    mainToolBar->installEventFilter(this);
#endif

    // ==== TOOLBAR BUTTONS
    mainToolBar->addWidget(new DuToolBarSpacer());
    title = new QLabel(STR_FILEDESCRIPTION);
    title->setAttribute(Qt::WA_TransparentForMouseEvents);
    mainToolBar->addWidget(title);

    mainToolBar->addWidget(new DuToolBarSpacer());

    //hide
    QToolButton *hideButton = new QToolButton();
    if (useSysTray)
    {
        hideButton->setIcon(QIcon(":/icons/hide"));
        hideButton->setObjectName("windowButton");
        mainToolBar->addWidget(hideButton);
        hideButton->setFixedSize(24,24);
    }

#ifndef Q_OS_LINUX

    m_minimizeAction = new QAction(this);
    m_minimizeAction->setIcon(QIcon(":/icons/minimize"));
    mainToolBar->addAction(m_minimizeAction);
    QWidget *minWidget = mainToolBar->widgetForAction(m_minimizeAction);
    minWidget->setFixedSize(24,24);
    minWidget->setObjectName("windowButton");
    mainToolBar->layout()->setAlignment(minWidget, Qt::AlignTop);

#ifndef Q_OS_MAC
    m_maximizeAction = new QAction(this);
    QIcon maximizeIcon;
    maximizeIcon.addFile(":/icons/maximize", QSize(), QIcon::Normal, QIcon::Off);
    maximizeIcon.addFile(":/icons/unmaximize", QSize(), QIcon::Normal, QIcon::On);
    m_maximizeAction->setIcon(maximizeIcon);
    m_maximizeAction->setCheckable(true);
    mainToolBar->addAction(m_maximizeAction);
    QWidget *maxWidget = mainToolBar->widgetForAction(m_maximizeAction);
    maxWidget->setFixedSize(24,24);
    maxWidget->setObjectName("windowButton");
    mainToolBar->layout()->setAlignment(maxWidget, Qt::AlignTop);
#endif // NOT MAC

    m_closeAction = new QAction(this);
    m_closeAction->setIcon(QIcon(":/icons/close-simple"));
    mainToolBar->addAction(m_closeAction);
    QWidget *closeWidget = mainToolBar->widgetForAction(m_closeAction);
    closeWidget->setFixedSize(24,24);
    closeWidget->setObjectName("windowButton");
    mainToolBar->layout()->setAlignment(closeWidget, Qt::AlignTop);

#endif // Not LINUX

    // ===== STATUSBAR ======

    // version in statusbar
    mainStatusBar->addPermanentWidget(new QLabel("v" + QString(STR_VERSION)));
    duqf_settingsButton = new QToolButton();
    duqf_settingsButton->setIcon(QIcon(":/icons/settings-w"));
    duqf_settingsButton->setToolTip("Go to Settings");
    duqf_settingsButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(duqf_settingsButton);
    QToolButton *helpButton = new QToolButton();
    helpButton->setIcon(QIcon(":/icons/help"));
    helpButton->setToolTip("Get Help");
    helpButton->setPopupMode( QToolButton::InstantPopup );
    helpMenu = new QMenu(this);

    helpButton->setMenu(helpMenu);
    mainStatusBar->addPermanentWidget(helpButton);

    if (QString(URL_DOC) != "")
    {
        QAction *docAction = new QAction(QIcon(":/icons/documentation"), "Help");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        connect(docAction, SIGNAL(triggered()), this, SLOT(duqf_doc()));
    }
    QAction *aboutAction = new QAction(QIcon(":/icons/info"), "About");
    helpMenu->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(duqf_about()));
    helpMenu->addSeparator();
    bool chat = QString(URL_CHAT) != "";
    bool bugReport = QString(URL_BUGREPORT) != "";
    bool forum = QString(URL_FORUM) != "";
    bool donate = QString(URL_DONATION) != "";
    if (bugReport)
    {
        QAction *bugReportAction = new QAction(QIcon(":/icons/bug-report"), "Bug Report");
        bugReportAction->setToolTip("Report a bug");
        helpMenu->addAction(bugReportAction);
        if (!chat && !forum && !donate) helpMenu->addSeparator();
        connect(bugReportAction, SIGNAL(triggered()), this, SLOT(duqf_bugReport()));
    }
    if (chat)
    {
        QAction *chatAction = new QAction(QIcon(":/icons/chat"), "Chat");
        chatAction->setToolTip("Come and have a chat");
        helpMenu->addAction(chatAction);
        if (!forum && !donate) helpMenu->addSeparator();
        connect(chatAction, SIGNAL(triggered()), this, SLOT(duqf_chat()));
    }
    if (forum)
    {
        QAction *forumAction = new QAction(QIcon(":/icons/forum"), "Forum");
        forumAction->setToolTip("Join us on our forum");
        helpMenu->addAction(forumAction);
        if (!donate) helpMenu->addSeparator();
        connect(forumAction, SIGNAL(triggered()), this, SLOT(duqf_forum()));
    }
    if (donate)
    {
        QAction *donateAction = new QAction(QIcon(":/icons/donate"), "I ♥ " + QString(STR_FILEDESCRIPTION));
        donateAction->setToolTip("Help us, donate now!");
        helpMenu->addAction(donateAction);
        helpMenu->addSeparator();
        connect(donateAction, SIGNAL(triggered()), this, SLOT(duqf_donate()));

        /*QToolButton *donateButton = new QToolButton();
        donateButton->setIcon(QIcon(":/icons/donate"));
        donateButton->setToolTip("I ♥ " + QString(STR_FILEDESCRIPTION));
        mainStatusBar->addPermanentWidget(donateButton);
        connect(donateButton, SIGNAL(clicked()), this, SLOT(duqf_donate()));*/
    }
    QAction *aboutQtAction = new QAction(QIcon(":/icons/qt"), "About Qt");
    helpMenu->addAction(aboutQtAction);

    // Check for update
    // moved in main()
    //duqf_checkUpdate();

    // ========= SETTINGS ========

    settingsWidget = new SettingsWidget();
    settingsWidget->titleBar()->setObjectName("settingsToolBar");
    duqf_settingsLayout->addWidget(settingsWidget);

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    DuQFUpdateSettingsWidget *usw = new DuQFUpdateSettingsWidget();
    settingsWidget->addPage(usw, "Updates", QIcon(":/icons/update-settings"));

    // ====== CONNECTIONS ======
    if (useSysTray)
    {
        connect(hideButton, SIGNAL(clicked()), this, SLOT(duqf_showHide()));
    }

    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(settingsWidget, SIGNAL(closeRequested()), this, SLOT(duqf_closeSettings()));
    connect(settingsWidget, SIGNAL(reinitRequested()), this, SLOT(duqf_reinitSettings()));

    DuApplication *app = qobject_cast<DuApplication*>(qApp);
    duqf_updateAvailable(app->updateInfo());

#ifndef Q_OS_LINUX
    connect(m_minimizeAction, &QAction::triggered, this, &MainWindow::minimizeTriggered);
#ifndef Q_OS_MAC
    connect(m_maximizeAction, &QAction::triggered, this, &MainWindow::maximizeTriggered);
#endif // Not mac
    connect(m_closeAction, &QAction::triggered, this, &MainWindow::close);
    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onQuit);
#endif // Not linux
}

void MainWindow::duqf_setStyle()
{
    // ======== STYLE ========

    //Re-set StyleSheet
    QSettings settings;
    QString cssFile = settings.value("appearance/cssFile", ":/styles/default").toString();
    QString style = settings.value("appearance/style","Default").toString();
    if (cssFile != "")
    {
        DuUI::updateCSS(cssFile);
    }
    else
    {
        DuUI::updateCSS("");
        qApp->setStyle(QStyleFactory::create(style));
    }
    //and font
    DuUI::setFont(settings.value("appearance/font", "Ubuntu").toString());
    //and tool buttons
    //int styleIndex = settings.value("appearance/toolButtonStyle", 2).toInt();
    //DuUI::setToolButtonStyle(styleIndex);
}

void MainWindow::duqf_raise()
{
    qDebug() << "Hello!";
    this->show();
    this->setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    this->raise();  // for MacOS
    this->activateWindow(); // for Windows
}

void MainWindow::duqf_bugReport()
{
    QDesktopServices::openUrl ( QUrl( URL_BUGREPORT ) );
}

void MainWindow::duqf_forum()
{
    QDesktopServices::openUrl ( QUrl( URL_FORUM ) );
}

void MainWindow::duqf_chat()
{
    QDesktopServices::openUrl ( QUrl( URL_CHAT ) );
}

void MainWindow::duqf_doc()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void MainWindow::duqf_donate()
{
    QDesktopServices::openUrl ( QUrl( URL_DONATION ) );
}

void MainWindow::duqf_settings(bool checked)
{
    duqf_settingsButton->setChecked(checked);
    if (checked)
    {
        mainStack->setCurrentIndex(Settings);
    }
    else
    {
        mainStack->setCurrentIndex(Home);
    }
}

void MainWindow::duqf_closeSettings()
{
    duqf_settings(false);
}

void MainWindow::duqf_reinitSettings()
{
    QMessageBox::StandardButton choice = QMessageBox::question(this, "Reset settings", "This will reset all settings to their default values and restart the application.\nAre you sure you want to continue?" );
    if (choice == QMessageBox::Yes)
    {
        QSettings settings;
        settings.clear();
        settings.sync();
        this->close();
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments());
    }
}

void MainWindow::duqf_about()
{
    duqf_aboutDialog->show();
}

void MainWindow::duqf_trayClicked(QSystemTrayIcon::ActivationReason reason)
{

    if (reason == QSystemTrayIcon::Trigger)
    {
        duqf_showHide();
    }
}

void MainWindow::duqf_showHide()
{
    if (this->isVisible())
    {
        duqf_actionShowHide->setIcon(QIcon(":/icons/show-dark"));
        duqf_actionShowHide->setText("Show " + QString(STR_INTERNALNAME));
        emit hideTriggered();
        this->hide();
    }
    else
    {
        duqf_actionShowHide->setIcon(QIcon(":/icons/hide-dark"));
        duqf_actionShowHide->setText("Hide " + QString(STR_INTERNALNAME));
        emit showTriggered();
        this->show();
    }
}

void MainWindow::duqf_askBeforeClose()
{
    QMessageBox::StandardButton r = QMessageBox::question(this, "Quitting Ramses", "Are you sure you want to quit Ramses?");
    if (r == QMessageBox::Yes) this->close();
}

void MainWindow::duqf_updateAvailable(QJsonObject updateInfo)
{
    // Check funding
    bool donate = QString(URL_DONATION) != "";
    if (donate)
    {
        double month = updateInfo.value("monthlyFund").toDouble(0.0);
        double goal = updateInfo.value("fundingGoal").toDouble(4000);
        if (goal > 0) {
            double ratio = month / goal * 100;
            if (!duqf_fundingBar)
            {
                duqf_fundingBar = new QProgressBar(this);
                duqf_fundingBar->setObjectName("fundingBar");
                duqf_fundingBar->setMaximumWidth(75);
                duqf_fundingBar->setFormat("♥ Donate");
                duqf_fundingBar->installEventFilter(this);
                mainStatusBar->addPermanentWidget(duqf_fundingBar);
            }
            duqf_fundingBar->setMaximum(goal);
            duqf_fundingBar->setValue(month);
            duqf_fundingBar->setToolTip( tr("This month, we've collected $%1.\n"
                                            "That's %2 % of our monthly goal.\n"
                                            "Thanks for your support!\n\n"
                                            "Click to Donate now!\nor go to: %3").arg(
                                             QString::number(month),
                                             QString::number(ratio, 'f', 0),
                                             QString(URL_DONATION)
                                             ));
        }
    }
}

void MainWindow::log(DuQFLog m)
{
    QString message = m.message();
    if (message == "") return;

    DuQFLog::LogType type = m.type();

    if (type == DuQFLog::Information) mainStatusBar->showMessage(message,5000);
    else if (type == DuQFLog::Warning) mainStatusBar->showMessage(message,10000);
    else if (type == DuQFLog::Critical) mainStatusBar->showMessage(message);
    else if (type == DuQFLog::Fatal) mainStatusBar->showMessage(message);
}

void MainWindow::pageChanged(int i)
{
    actionAdmin->setChecked(i == 3);
    duqf_settingsButton->setChecked(i == 1);
    actionSettings->setChecked(i == 1);
    actionLogIn->setChecked(i == 0);
    ui_propertiesDockWidget->hide();
}

void MainWindow::serverSettings()
{
    mainStack->setCurrentIndex(Settings);
    settingsWidget->setCurrentIndex(2);
}

void MainWindow::setOfflineAction()
{
    DBInterface::instance()->setOffline();
}

void MainWindow::setOnlineAction()
{
    DBInterface::instance()->setOnline();

    // Trigger a full sync
    if (RamServerInterface::instance()->isOnline()) DBInterface::instance()->fullSync();
}

void MainWindow::databaseSettingsAction()
{
    QString dataFilePath = DBInterface::instance()->dataFile();
    if (dataFilePath == "") return;

    QFileInfo dataFile(dataFilePath);
    if (!dataFile.exists()) return;

    if (!ui_databaseEditWidget)
    {
        ui_databaseEditWidget = new DatabaseEditWidget();
        connect(ui_databaseEditWidget, &DatabaseEditWidget::applied, this, &MainWindow::hidePropertiesDock);
    }

    ui_databaseEditWidget->setDbFile( dataFilePath );
    this->setPropertiesDockWidget( ui_databaseEditWidget, tr("Edit %1").arg(dataFile.baseName()), ":/icons/storage" );
}

void MainWindow::home()
{
    mainToolBar->show();
    mainStack->setCurrentIndex(Home);
}

void MainWindow::userProfile()
{
    mainStack->setCurrentIndex(UserProfile);
}

void MainWindow::revealAdminFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::AdminFolder), true );
}

void MainWindow::revealPreProdFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::PreProdFolder), true );
}

void MainWindow::revealProdFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::ProdFolder), true );
}

void MainWindow::revealPostProdFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::PostProdFolder), true );
}

void MainWindow::revealAssetsFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::AssetsFolder), true );
}

void MainWindow::revealShotsFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::ShotsFolder), true );
}

void MainWindow::revealOutputFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::OutputFolder), true );
}

void MainWindow::revealUserFolder()
{
    RamUser *current = Ramses::instance()->currentUser();
    if (current) FileUtils::openInExplorer( current->path(), true );
}

void MainWindow::revealVersionsFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::VersionsFolder), true );
}

void MainWindow::revealTrashFolder()
{
    RamProject *proj = Ramses::instance()->currentProject();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::TrashFolder), true );
}

void MainWindow::admin(bool show)
{
    mainToolBar->show();
    if (show) mainStack->setCurrentIndex(Admin);
    else home();
}

void MainWindow::pipeline()
{
    mainToolBar->show();
    mainStack->setCurrentIndex(PipeLine);
}

void MainWindow::shots()
{
    mainToolBar->show();
    mainStack->setCurrentIndex(Shots);
}

void MainWindow::assets()
{
    mainToolBar->show();
    mainStack->setCurrentIndex(Assets);
}

void MainWindow::schedule()
{
    mainToolBar->show();
    mainStack->setCurrentIndex(Schedule);
}

void MainWindow::currentUserChanged()
{
    disconnect(_currentUserConnection);

    //defaults
    ui_userButton->setText("Guest");
    ui_userButton->setIcon(QIcon(""));
    actionAdmin->setVisible(false);
    actionAdmin->setChecked(false);
    actionUserProfile->setVisible(false);
    actionUserFolder->setVisible(false);

    RamUser *user = Ramses::instance()->currentUser();
    if (!user)
    {
        actionLogIn->setVisible(true);
        actionLogOut->setVisible(false);
        actionSettings->setVisible(true);
        ui_networkButton->setVisible(false);
        ui_projectSelectorAction->setVisible(false);
        home();
        return;
    }

    actionLogIn->setVisible(false);
    actionLogOut->setVisible(true);
    ui_networkButton->setVisible(true);
    actionSettings->setVisible(false);
    ui_projectSelectorAction->setVisible(true);

    _currentUserConnection = connect(user, &RamUser::dataChanged, this, &MainWindow::currentUserChanged);

    ui_userButton->setText(user->shortName());
    actionUserProfile->setVisible(true);
    actionUserFolder->setVisible(true);

    if (user->role() == RamUser::Admin)
    {
        actionAdmin->setVisible(true);
        ui_userButton->setIcon(QIcon(":/icons/admin"));
    }
    else if (user->role() == RamUser::ProjectAdmin)
    {
        ui_userButton->setIcon(QIcon(":/icons/project-admin"));
    }
    else if (user->role() == RamUser::Lead)
    {
        ui_userButton->setIcon(QIcon(":/icons/lead"));
    }
    else
    {
        ui_userButton->setIcon(QIcon(":/icons/user"));
    }
}

void MainWindow::currentProjectChanged(RamProject *project)
{
    ui_pipelineMenuAction->setVisible(false);
    ui_shotMenuAction->setVisible(false);
    ui_assetMenuAction->setVisible(false);
    ui_scheduleMenuAction->setVisible(false);
    ui_filesMenuAction->setVisible(false);

    ui_projectEditWiget->setObject(project);

    if (!project) {
        ui_statsTitle->setTitle( "Project" );
        home();
    }
    else {
        RamUser *user = Ramses::instance()->currentUser();
        if (!user) return;

        ui_statsTitle->setTitle( project->name() );

        ui_shotMenuAction->setVisible(true);
        ui_assetMenuAction->setVisible(true);
        ui_scheduleMenuAction->setVisible(true);
        ui_filesMenuAction->setVisible(true);

        if (user->role() == RamUser::Admin)
        {
            ui_pipelineMenuAction->setVisible(true);
        }
        else if (user->role() == RamUser::ProjectAdmin)
        {
            ui_pipelineMenuAction->setVisible(true);
        }
    }
}

void MainWindow::freezeUI(bool f)
{
    if (f)
    {
        m_currentPageIndex = mainStack->currentIndex();
        mainToolBar->hide();
        mainStack->setCurrentIndex(Progress);
    }
    else
    {
        mainStack->setCurrentIndex(m_currentPageIndex);
        mainToolBar->show();
    }
    this->repaint();
}

void MainWindow::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    QString address =  RamServerInterface::instance()->serverAddress();
    if (s == NetworkUtils::Online)
    {
        ui_refreshButton->setVisible(true);
        ui_networkButton->setText(address);
        actionSetOnline->setVisible(false);
        actionSetOffline->setVisible(true);

        if (RamServerInterface::instance()->ssl())
        {
            ui_networkButton->setIcon(QIcon(":/icons/shield"));
            ui_networkButton->setToolTip("Connected.");
            ui_networkButton->setStatusTip("");
        }
        else
        {
            ui_networkButton->setIcon(QIcon(":/icons/no-shield"));
            ui_networkButton->setToolTip("WARNING: Connection is not secured!");
            ui_networkButton->setStatusTip("WARNING: Connection is not secured!");
        }
    }
    else if (s == NetworkUtils::Connecting) ui_networkButton->setText("Connecting to " + address);
    else if (s == NetworkUtils::Offline)
    {
        ui_refreshButton->setVisible(false);
        ui_networkButton->setText("Offline");
        ui_networkButton->setIcon(QIcon(":/icons/storage"));
        ui_networkButton->setToolTip("Offline.");
        ui_networkButton->setStatusTip("");
        actionSetOnline->setVisible(true);
        actionSetOffline->setVisible(false);
    }
}

void MainWindow::finishSync()
{
    if (m_closing) {
        m_readyToClose = true;
        this->close();
        return;
    }

    ui_refreshButton->show();

    // Refresh all UI!
    this->update();

    mainStatusBar->showMessage(tr("Sync finished!"), 5000);
}

void MainWindow::startSync()
{
    ui_refreshButton->hide();
    mainStatusBar->showMessage(tr("Syncing..."));
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() == "mainToolBar")
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->button() == Qt::LeftButton)
            {
                duqf_toolBarClicked = true;
                duqf_dragPosition = mouseEvent->globalPos() - this->frameGeometry().topLeft();
                event->accept();
            }
            return true;
        }

        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mouseEvent = (QMouseEvent*)event;
            if (mouseEvent->buttons() & Qt::LeftButton)
            {
                if (duqf_toolBarClicked)
                {
                    if (this->isMaximized()) return false;
                    this->move(mouseEvent->globalPos() - duqf_dragPosition);
                    event->accept();
                }
                return true;
            }
            return false;
        }

        if (event->type() == QEvent::MouseButtonRelease)
        {
            duqf_toolBarClicked = false;
            return false;
        }
    }
    else if (obj->objectName() == "fundingBar")
    {
        if (event->type() == QEvent::MouseButtonPress) {
            duqf_donate();
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setBrush(palette().window());
    QPen pen;
    pen.setColor(palette().window().color());
    pen.setWidth(2);
    painter.setPen(pen);

#ifndef Q_OS_LINUX
    //painter.setBrush(QBrush(Qt::red));
    if (!m_maximized) {
        int margin = 0;
        auto m = QMargins(margin, margin, margin, margin);
        painter.drawRoundedRect(rect().marginsRemoved(m), 10, 10);
    }
    else {
        painter.drawRect(rect());
    }
#else
    painter.drawRect(rect());
#endif

    //painter.setRenderHint(QPainter::Antialiasing, false);

    QMainWindow::paintEvent(e);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (m_closing && !m_readyToClose)
    {
        QMessageBox::StandardButton r = QMessageBox::question(this,
                              tr("Closing Ramses..."),
                              tr("I'm already closing, do you want me to force quit?\n\nThis may cause some data loss if the sync is not finished yet.")
                              );
        if (r == QMessageBox::Yes) m_readyToClose = true;
    }

    if (!m_readyToClose)
    {
        // Get to the home page first to make sure all toolbars are hidden
        home();

        // Let's save the ui state
        DuSettingsManager::instance()->saveUIWindowState(
            this->saveGeometry(),
            this->saveState()
            );

        if (DBInterface::instance()->connectionStatus() == NetworkUtils::Online)
        {
            // Clean before quit!
            m_closing = true;

            ProgressManager *pm = ProgressManager::instance();
            pm->setTitle("Disconnecting...");
            pm->setText("One last sync!");
            pm->setMaximum(3);
            pm->start();
            pm->freeze();

            DBInterface::instance()->setOffline();

            event->ignore();
        }
        else m_readyToClose = true;
    }

    if (m_readyToClose)
    {
        emit closeTriggered();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift)
    {
        m_shiftPressed = true;
    }
    QMainWindow::keyPressEvent(key);
}

void MainWindow::keyReleaseEvent(QKeyEvent *key)
{
    if(key->key() == Qt::Key_Shift)
    {
        m_shiftPressed = false;
    }
    QMainWindow::keyReleaseEvent(key);
}
