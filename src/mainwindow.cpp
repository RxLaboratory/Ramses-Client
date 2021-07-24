#include "mainwindow.h"
#include "config.h"

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
    ProcessManager::instance();
    // Ramses
    Ramses::instance();
    // The ramses loader
    RamLoader::instance();

    qDebug() << "> Setting up menus";

    // Populate Toolbar
    actionAdmin->setVisible(false);
    actionProjectSettings->setVisible(false);
    actionPipeline->setVisible(false);
    actionShots->setVisible(false);
    actionAssets->setVisible(false);
    actionSchedule->setVisible(false);
    actionStatistics->setVisible(false);

    mainToolBar->addWidget(new ToolBarSpacer(this));

    mainToolBar->addWidget(new ProjectSelectorWidget(this));

    //Populate status bar

    mainStatusBar->addPermanentWidget(new ProgressBar(this));

    ui_refreshButton = new QToolButton(this);
    ui_refreshButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui_refreshButton->setText("");
    ui_refreshButton->setIcon(QIcon(":/icons/reload"));
    mainStatusBar->addPermanentWidget(ui_refreshButton);
    ui_refreshButton->hide();

    ui_consoleButton = new QToolButton(this);
    ui_consoleButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui_consoleButton->setText("");
    ui_consoleButton->setIcon(QIcon(":/icons/bash"));
    ui_consoleButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(ui_consoleButton);

    mainStatusBar->addPermanentWidget(new DuQFLogToolButton(this));

    ui_networkButton = new QToolButton(this);
    ui_networkButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    ui_networkButton->setText("Offline");
    ui_networkButton->setMinimumWidth(50);
    mainStatusBar->addPermanentWidget(ui_networkButton);

    ui_userMenu = new QMenu();
    ui_userMenu->addAction(actionLogIn);
    ui_userMenu->addAction(actionUserFolder);
    actionUserFolder->setVisible(false);
    ui_userMenu->addAction(actionUserProfile);
    actionUserProfile->setVisible(false);
    ui_userMenu->addAction(actionLogOut);
    actionLogOut->setVisible(false);
    ui_userButton = new QToolButton();
    ui_userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_userButton->setText("Guest");
    ui_userButton->setMinimumWidth(75);
    ui_userButton->setMenu(ui_userMenu);
    ui_userButton->setPopupMode(QToolButton::InstantPopup);
    mainStatusBar->addPermanentWidget(ui_userButton);

    // Add default stuff
    duqf_initUi();

    qDebug() << "> Loading settings";

    // Add settings
    LocalSettingsWidget *lsw = new LocalSettingsWidget(this);
    settingsWidget->addPage(lsw, "Ramses data", QIcon(":/icons/storage"));
    ServerSettingsWidget *csw = new ServerSettingsWidget(this);
    settingsWidget->addPage(csw, "Server", QIcon(":/icons/server-settings"));
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
    SettingsWidget *adminPage = new SettingsWidget("Administration", this);
    adminPage->showReinitButton(false);
    mainStack->addWidget(adminPage);
    // Admin tabs
    qDebug() << "> Admin";
    adminPage->addPage(new UserListManagerWidget(this),"Users", QIcon(":/icons/users"));
    qDebug() << "  > users ok";
    adminPage->addPage(new ProjectListManagerWidget(this), "Projects", QIcon(":/icons/projects"));
    qDebug() << "  > projects ok";
    adminPage->addPage(new TemplateStepListManagerWidget(this), "Template Steps", QIcon(":/icons/steps"));
    qDebug() << "  > template steps ok";
    adminPage->addPage(new TemplateAssetGroupListManagerWidget(this), "Template Asset Groups", QIcon(":/icons/asset-groups"));
    qDebug() << "  > template assets ok";
    adminPage->addPage(new StateListManagerWidget(this), "States", QIcon(":/icons/state"));
    qDebug() << "  > states ok";
    adminPage->addPage(new FileTypeListManagerWidget(this), "File Types", QIcon(":/icons/files"));
    qDebug() << "  > file types ok";
    adminPage->addPage(new ApplicationListManagerWidget(this), "Applications", QIcon(":/icons/applications"));
    qDebug() << "  > applications ok";//*/

    // Project settings
    SettingsWidget *projectSettingsPage = new SettingsWidget("Project Administration", this);
    projectSettingsPage->showReinitButton(false);
    mainStack->addWidget(projectSettingsPage);
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
    projectSettingsPage->addPage( pSettingsWidget, "Settings", QIcon(":/icons/projects"));
    qDebug() << "  > project settings ok";
    projectSettingsPage->addPage(new StepListManagerWidget(this), "Steps", QIcon(":/icons/steps"));
    qDebug() << "  > steps ok";
    projectSettingsPage->addPage(new PipeFileListManagerWidget(this), "Pipe Types", QIcon(":/icons/pipe-files"));
    qDebug() << "  > pipe types ok";
    projectSettingsPage->addPage(new AssetGroupListManagerWidget(this), "Asset Groups", QIcon(":/icons/asset-groups"));
    qDebug() << "  > asset groups ok";
    projectSettingsPage->addPage(new AssetListManagerWidget(this), "Assets", QIcon(":/icons/assets"));
    qDebug() << "  > assets ok";
    projectSettingsPage->addPage(new SequenceListManagerWidget(this), "Sequences", QIcon(":/icons/sequences"));
    qDebug() << "  > sequences ok";
    projectSettingsPage->addPage(new ShotListManagerWidget(this), "Shots", QIcon(":/icons/shots"));
    qDebug() << "  > shots ok";//*/

    // Pipeline editor
#ifndef DEACTIVATE_PIPELINE
    PipelineWidget *pipelineEditor = new PipelineWidget(this);
    mainStack->addWidget(pipelineEditor);
    connect(pipelineEditor, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Pipeline ready";//*/
#endif

#ifndef DEACTIVATE_ASSETSTABLE
    ItemTableManagerWidget *assetsTable = new ItemTableManagerWidget(RamStep::AssetProduction, this);
    mainStack->addWidget(assetsTable);
    connect(assetsTable, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Assets table ready";
#endif

#ifndef DEACTIVATE_SHOTSTABLE
    ItemTableManagerWidget *shotsTable = new ItemTableManagerWidget(RamStep::ShotProduction, this);
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
    ui_statsTitle = new DuQFDockTitle("Statistics", this);
    ui_statsTitle->setObjectName("dockTitle");
    ui_statsTitle->setIcon(":/icons/stats");
    ui_statsDockWidget->setTitleBarWidget(ui_statsTitle);
    ui_statsDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_statsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_statsDockWidget->setWidget( statsTable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_statsDockWidget);

    qDebug() << "> Statistics table ready";
#endif

    // A console in a tab
    DuQFLoggingTextEdit *console = new DuQFLoggingTextEdit(this);
    ui_consoleDockWidget = new QDockWidget("Console");
    DuQFDockTitle *consoleTitle = new DuQFDockTitle("Console", this);
    consoleTitle->setObjectName("dockTitle");
    consoleTitle->setIcon(":/icons/bash");
    ui_consoleDockWidget->setTitleBarWidget(consoleTitle);
    ui_consoleDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_consoleDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_consoleDockWidget->setWidget( console );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_consoleDockWidget);
    this->tabifyDockWidget( ui_statsDockWidget, ui_consoleDockWidget) ;

    qDebug() << "> Console dock ready";


    // Hide docks
    ui_statsDockWidget->hide();
    ui_consoleDockWidget->hide();

    // Progress page
    progressPage = new ProgressPage(this);
    mainStack->addWidget(progressPage);

    // Set UI
    mainStack->setCurrentIndex(0);

    qDebug() << "> Connecting events";

    // Connect events
    connect(ProcessManager::instance(), &ProcessManager::freezeUI, this, &MainWindow::freezeUI);
    connect(actionLogIn,SIGNAL(triggered()), this, SLOT(loginAction()));
    connect(actionLogOut,SIGNAL(triggered()), this, SLOT(logoutAction()));
    connect(actionUserProfile,SIGNAL(triggered()), this, SLOT(userProfile()));
    connect(actionUserFolder,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(actionAdmin,SIGNAL(triggered(bool)), this, SLOT(admin(bool)));
    connect(actionProjectSettings,SIGNAL(triggered(bool)), this, SLOT(projectSettings(bool)));
    connect(actionPipeline, SIGNAL(triggered(bool)), this, SLOT(pipeline(bool)));
    connect(actionShots,SIGNAL(triggered(bool)), this, SLOT(shots(bool)));
    connect(actionAssets,SIGNAL(triggered(bool)), this, SLOT(assets(bool)));
    connect(actionSchedule,SIGNAL(triggered(bool)), this, SLOT(schedule(bool)));
    connect(actionStatistics,SIGNAL(triggered(bool)), ui_statsDockWidget, SLOT(setVisible(bool)));
    connect(ui_statsDockWidget,SIGNAL(visibilityChanged(bool)), actionStatistics, SLOT(setChecked(bool)));
    connect(ui_consoleDockWidget,SIGNAL(visibilityChanged(bool)), ui_consoleButton, SLOT(setChecked(bool)));
    connect(ui_consoleButton,SIGNAL(clicked(bool)), ui_consoleDockWidget, SLOT(setVisible(bool)));
    connect(adminPage, SIGNAL(closeRequested()), this, SLOT(home()));
    connect(projectSettingsPage, SIGNAL(closeRequested()), this, SLOT(home()));
    connect(ui_networkButton,SIGNAL(clicked()),this, SLOT(networkButton_clicked()));
    connect(ui_refreshButton, SIGNAL(clicked()), Ramses::instance(), SLOT(refresh()));
    connect(mainStack,SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));
    connect(lp, &LoginPage::serverSettings, this, &MainWindow::serverSettings);
    connect(DuQFLogger::instance(), &DuQFLogger::newLog, this, &MainWindow::log);
    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::raise);
    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::show);
    connect(Ramses::instance(),&Ramses::loggedIn, this, &MainWindow::loggedIn);
    connect(Ramses::instance(),&Ramses::loggedOut, this, &MainWindow::loggedOut);
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
    connect(DBInterface::instance(),&DBInterface::data, RamLoader::instance(), &RamLoader::newData );
    connect(DBInterface::instance(),&DBInterface::connectionStatusChanged, this, &MainWindow::dbiConnectionStatusChanged);

    // Set style

    qDebug() << "Setting CSS";

    duqf_setStyle();
}

void MainWindow::addObjectDockWidget(ObjectDockWidget *w)
{
    this->addDockWidget(Qt::RightDockWidgetArea, w);
    connect(w, &QObject::destroyed, this, &MainWindow::dockWidgetDestroyed);
    w->installEventFilter(this);
}

void MainWindow::duqf_initUi()
{
    // ===== SYSTRAY ======
    duqf_actionShowHide = new QAction("Hide " + QString(STR_INTERNALNAME), this);
    duqf_actionShowHide->setIcon(QIcon(":/icons/hide-dark"));
    bool useSysTray = QSystemTrayIcon::isSystemTrayAvailable() && USE_SYSTRAY;
    if (useSysTray)
    {
        QMenu *trayMenu = new QMenu(QString(STR_INTERNALNAME),this);
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

    // remove right click on toolbar
    mainToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    // set frameless
#ifndef Q_OS_MAC
    this->setWindowFlags(Qt::FramelessWindowHint);
#endif
    //drag window
    duqf_toolBarClicked = false;
    mainToolBar->installEventFilter(this);

    // ==== TOOLBAR BUTTONS
    mainToolBar->addWidget(new ToolBarSpacer());
    title = new QLabel(STR_FILEDESCRIPTION);
    mainToolBar->addWidget(title);
    //minimize
#ifndef Q_OS_MAC
    QToolButton *minimizeButton = new QToolButton();
    minimizeButton->setIcon(QIcon(":/icons/minimize"));
    minimizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(minimizeButton);
#endif
    //maximize
    duqf_maximizeButton = new QToolButton(this);
    duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
    duqf_maximizeButton->setObjectName("windowButton");
    mainToolBar->addWidget(duqf_maximizeButton);
    //hide
    QToolButton *hideButton = hideButton = new QToolButton();
    if (useSysTray)
    {
        hideButton->setIcon(QIcon(":/icons/hide"));
        hideButton->setObjectName("windowButton");
        mainToolBar->addWidget(hideButton);
    }
    //quit
    QToolButton *quitButton = new QToolButton(this);
    quitButton->setIcon(QIcon(":/icons/quit"));
    quitButton->setObjectName("windowButton");
    mainToolBar->addWidget(quitButton);

    // ===== STATUSBAR ======

    // version in statusbar
    mainStatusBar->addPermanentWidget(new QLabel("v" + QString(STR_VERSION)));
    duqf_settingsButton = new QToolButton();
    duqf_settingsButton->setIcon(QIcon(":/icons/settings"));
    duqf_settingsButton->setToolTip("Go to Settings");
    duqf_settingsButton->setCheckable(true);
    mainStatusBar->addPermanentWidget(duqf_settingsButton);
    QToolButton *helpButton = new QToolButton();
    helpButton->setIcon(QIcon(":/icons/help"));
    helpButton->setToolTip("Get Help");
    helpButton->setPopupMode( QToolButton::InstantPopup );
    helpMenu = new QMenu(this);
    if (QString(URL_DOC) != "")
    {
        QAction *docAction = new QAction(QIcon(":/icons/documentation"), "Help");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        connect(docAction, SIGNAL(triggered()), this, SLOT(duqf_doc()));
    }
    QAction *aboutAction = new QAction(QIcon(":/icons/about"), "About");
    helpMenu->addAction(aboutAction);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(duqf_about()));
    helpMenu->addSeparator();
    bool chat = QString(URL_CHAT) != "";
    bool bugReport = QString(URL_BUGREPORT) != "";
    bool forum = QString(URL_FORUM) != "";
    if (bugReport)
    {
        QAction *bugReportAction = new QAction(QIcon(":/icons/bug-report"), "Bug Report");
        bugReportAction->setToolTip("Report a bug");
        helpMenu->addAction(bugReportAction);
        if (!chat && !forum) helpMenu->addSeparator();
        connect(bugReportAction, SIGNAL(triggered()), this, SLOT(duqf_bugReport()));
    }
    if (chat)
    {
        QAction *chatAction = new QAction(QIcon(":/icons/chat"), "Chat");
        chatAction->setToolTip("Come and have a chat");
        helpMenu->addAction(chatAction);
        if (!forum) helpMenu->addSeparator();
        connect(chatAction, SIGNAL(triggered()), this, SLOT(duqf_chat()));
    }
    if (forum)
    {
        QAction *forumAction = new QAction(QIcon(":/icons/forum"), "Forum");
        forumAction->setToolTip("Join us on our forum");
        helpMenu->addAction(forumAction);
        helpMenu->addSeparator();
        connect(forumAction, SIGNAL(triggered()), this, SLOT(duqf_forum()));
    }
    QAction *aboutQtAction = new QAction(QIcon(":/icons/qt"), "About Qt");
    helpMenu->addAction(aboutQtAction);

    helpButton->setMenu(helpMenu);
    mainStatusBar->addPermanentWidget(helpButton);

    // ========= SETTINGS ========

    settingsWidget = new SettingsWidget();
    duqf_settingsLayout->addWidget(settingsWidget);

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    // ====== CONNECTIONS ======
    connect(duqf_maximizeButton,SIGNAL(clicked()),this,SLOT(duqf_maximize()));
#ifndef Q_OS_MAC
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    if (useSysTray)
    {
        connect(quitButton, SIGNAL(clicked()), this, SLOT(duqf_askBeforeClose()));
        connect(hideButton, SIGNAL(clicked()), this, SLOT(duqf_showHide()));
    }
    else connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(settingsWidget, SIGNAL(closeRequested()), this, SLOT(duqf_closeSettings()));
    connect(settingsWidget, SIGNAL(reinitRequested()), this, SLOT(duqf_reinitSettings()));
}

void MainWindow::duqf_setStyle()
{
    // ======== STYLE ========

    //Re-set StyleSheet
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
    int styleIndex = settings.value("appearance/toolButtonStyle", 2).toInt();
    DuUI::setToolButtonStyle(styleIndex);
}

void MainWindow::duqf_maximize(bool max)
{
    if (!max)
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/maximize"));
        this->showNormal();
    }
    else
    {
        duqf_maximizeButton->setIcon(QIcon(":/icons/unmaximize"));
        this->showMaximized();
    }
}

void MainWindow::duqf_maximize()
{
    duqf_maximize(!this->isMaximized());
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

void MainWindow::duqf_settings(bool checked)
{
    duqf_settingsButton->setChecked(checked);
    if (checked)
    {
        mainStack->setCurrentIndex(1);
    }
    else
    {
        mainStack->setCurrentIndex(0);
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
        this->hide();
        duqf_actionShowHide->setIcon(QIcon(":/icons/show-dark"));
        duqf_actionShowHide->setText("Show " + QString(STR_INTERNALNAME));
    }
    else
    {
        this->show();
        duqf_actionShowHide->setIcon(QIcon(":/icons/hide-dark"));
        duqf_actionShowHide->setText("Hide " + QString(STR_INTERNALNAME));
    }
}

void MainWindow::duqf_askBeforeClose()
{
    QMessageBox::StandardButton r = QMessageBox::question(this, "Quitting Ramses", "Are you sure you want to quit Ramses?");
    if (r == QMessageBox::Yes) this->close();
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

void MainWindow::dockWidgetDestroyed(QObject *dockObj)
{
    ObjectDockWidget *dock = (ObjectDockWidget*)dockObj;
    if (dock)
    {
        _dockedObjects.removeAll(dock);
    }
}

void MainWindow::pageChanged(int i)
{
    actionAdmin->setChecked(i == 3);
    actionProjectSettings->setChecked(i == 4);
    actionPipeline->setChecked(i == 5);
    actionAssets->setChecked(i == 6);
    actionShots->setChecked(i == 7);
    actionSchedule->setChecked(i == 8);
}

void MainWindow::serverSettings()
{
    mainStack->setCurrentIndex(1);
    settingsWidget->setCurrentIndex(1);
}

void MainWindow::loginAction()
{
    mainStack->setCurrentIndex(0);
}

void MainWindow::logoutAction()
{
    Ramses::instance()->logout();
}

void MainWindow::home()
{
    mainStack->setCurrentIndex(0);
}

void MainWindow::userProfile()
{
    mainStack->setCurrentIndex(2);
}

void MainWindow::revealUserFolder()
{
    RamUser *current = Ramses::instance()->currentUser();
    if (current) FileUtils::openInExplorer( current->path() );
}

void MainWindow::admin(bool show)
{
    if (show) mainStack->setCurrentIndex(3);
    else home();
}

void MainWindow::projectSettings(bool show)
{
    if (show) mainStack->setCurrentIndex(4);
    else home();
}

void MainWindow::pipeline(bool show)
{
    if (show) mainStack->setCurrentIndex(5);
    else home();
}

void MainWindow::shots(bool show)
{
    if (show) mainStack->setCurrentIndex(7);
    else home();
}

void MainWindow::assets(bool show)
{
    if (show) mainStack->setCurrentIndex(6);
    else home();
}

void MainWindow::schedule(bool show)
{
    if (show) mainStack->setCurrentIndex(8);
    else home();
}

void MainWindow::networkButton_clicked()
{
    DBInterface *dbi = DBInterface::instance();
    if (dbi->connectionStatus() != NetworkUtils::Online) dbi->setOnline();
    else dbi->setOffline();
}

void MainWindow::loggedIn()
{
    actionLogIn->setVisible(false);
    actionLogOut->setVisible(true);
    currentUserChanged();
}

void MainWindow::loggedOut()
{
    actionLogIn->setVisible(true);
    actionLogOut->setVisible(false);
    currentUserChanged();
    if (mainStack->currentIndex() != 1) mainStack->setCurrentIndex(0);
}

void MainWindow::currentUserChanged()
{
    disconnect(_currentUserConnection);

    //defaults
    ui_userButton->setText("Guest");
    ui_userButton->setIcon(QIcon(""));
    actionAdmin->setVisible(false);
    actionAdmin->setChecked(false);
    actionProjectSettings->setVisible(false);
    actionProjectSettings->setChecked(false);
    actionUserProfile->setVisible(false);
    actionUserFolder->setVisible(false);
    actionPipeline->setVisible(false);
    actionPipeline->setChecked(false);
    actionShots->setVisible(false);
    actionShots->setChecked(false);
    actionAssets->setVisible(false);
    actionAssets->setChecked(false);
    actionSchedule->setVisible(false);
    actionStatistics->setVisible(false);

    RamUser *user = Ramses::instance()->currentUser();
    if (!user) return;

    _currentUserConnection = connect(user, &RamUser::changed, this, &MainWindow::currentUserChanged);

    ui_userButton->setText(user->shortName());
    actionUserProfile->setVisible(true);
    actionUserFolder->setVisible(true);

    actionShots->setVisible(true);
    actionAssets->setVisible(true);
    actionSchedule->setVisible(true);
    actionStatistics->setVisible(true);

    if (user->role() == RamUser::Admin)
    {
        actionAdmin->setVisible(true);
        actionProjectSettings->setVisible(true);
        actionPipeline->setVisible(true);
        ui_userButton->setIcon(QIcon(":/icons/admin"));
    }
    else if (user->role() == RamUser::ProjectAdmin)
    {
        actionProjectSettings->setVisible(true);
        actionPipeline->setVisible(true);
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
    ui_currentProjectSettings->setObject(project);

    if (!project)
    {
        ui_statsTitle->setTitle( "Project" );
        ui_statsDockWidget->hide();
        home();
    }
    else
        ui_statsTitle->setTitle( project->name() );
}

void MainWindow::freezeUI(bool f)
{
    if (f)
    {
        m_currentPageIndex = mainStack->currentIndex();
        mainStack->setCurrentIndex(8);
    }
    else
    {
        mainStack->setCurrentIndex(m_currentPageIndex);
    }
    this->repaint();
}

void MainWindow::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s == NetworkUtils::Online)
    {
        ui_refreshButton->show();
        ui_networkButton->setText("Online");
    }
    else if (s == NetworkUtils::Connecting) ui_networkButton->setText("Connecting...");
    else if (s == NetworkUtils::Offline)
    {
        ui_refreshButton->hide();
        ui_networkButton->setText("Offline");
    }
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

#ifndef Q_OS_MAC
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            duqf_maximize();
            event->accept();
            return true;
        }
#endif
    }

    ObjectDockWidget *o = qobject_cast<ObjectDockWidget*>(obj);
    if (o)
    {
        if (event->type() == QEvent::Show)
        {
            for (int i = 0; i < _dockedObjects.count(); i++)
            {
                ObjectDockWidget *other = _dockedObjects.at(i);
                if (!other) continue;

                if (other->objectType() == o->objectType() && other->isVisible())
                {
                    if (m_shiftPressed)
                        this->tabifyDockWidget( other, o);
                    else
                        other->hide();
                }
            }
            _dockedObjects << o;
            return false;
        }
        if (event->type() == QEvent::Hide)
        {
            _dockedObjects.removeAll(o);
            return false;
        }
    }

    return QMainWindow::eventFilter(obj, event);

}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DBInterface::instance()->suspend(true);
    QFontDatabase::removeAllApplicationFonts();
    trayIcon->hide();
    event->accept();
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
