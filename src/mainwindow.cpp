#include "mainwindow.h"

#include "assetgroupmanagerwidget.h"
#include "assetmanagerwidget.h"
#include "duqf-app/dusettingsmanager.h"
#include "itemmanagerwidget.h"
#include "pipefilemanagerwidget.h"
#include "progressmanager.h"
#include "docks/consolewidget.h"
#include "loginpage.h"
#include "sequencemanagerwidget.h"
#include "shotmanagerwidget.h"
#include "stepmanagerwidget.h"
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
#include "duqf-widgets/duqflogtoolbutton.h"
#include "duqf-app/app-version.h"
#include "docks/settingsdock.h"
#include "duqf-widgets/dutoolbarspacer.h"

MainWindow::MainWindow(const QCommandLineParser &cli, QWidget *parent) :
    DuMainWindow{parent}
{
    qDebug() << "> Initializing threads and workers";

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

    // Setup the actions
    setupActions();
    // Build the window
    setupUi();
    // Build the toolbar
    setupToolBar();
    // Build the sys tray icon
    setupSysTray();
    // Build the docks
    setupDocks();

    // Set style
    setStyle();

    // Set UI
    ui_mainStack->setCurrentIndex(Home);

    // Everything is ready
    connectEvents();

    // Restore UI state
    settings.beginGroup("ui");
    if (settings.value("maximized", false).toBool() )
        this->showMaximized();
    this->restoreState( settings.value("windowState").toByteArray() );
    settings.endGroup();

    // Process arguments

    // Load file
    QStringList args = cli.positionalArguments();
    for(const QString &filePath: args) {
        QFileInfo argInfo(filePath);
        if (argInfo.exists() && argInfo.suffix().toLower() == "ramses")
        {
            StateManager::i()->open(filePath);
            break;
        }
    }
}

void MainWindow::connectEvents()
{
    // Connect events
    connect(ProgressManager::instance(), &ProgressManager::freezeUI, this, &MainWindow::freezeUI);

    // Toolbar buttons
    connect(m_actionLogIn,SIGNAL(triggered()), this, SLOT(home()));
    connect(m_actionLogOut,SIGNAL(triggered()), StateManager::i(), SLOT(logout()));
    connect(m_actionSetOnline, &QAction::triggered, this, &MainWindow::setOnlineAction);
    connect(m_actionSetOffline, &QAction::triggered, this, &MainWindow::setOfflineAction);
    connect(m_actionDatabaseSettings, &QAction::triggered, this, &MainWindow::databaseSettingsAction);
    connect(m_actionUserProfile,&QAction::triggered, ui_userDockWidget, &DuDockWidget::show);
    connect(m_actionUserFolder,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(m_actionAdmin,SIGNAL(triggered()), this, SLOT(admin()));

    connect(m_actionPipeline,SIGNAL(triggered()), this, SLOT(pipeline()));
    connect(ui_pipelineButton,SIGNAL(clicked()), this, SLOT(pipeline()));

    connect(m_actionShots,SIGNAL(triggered(bool)), this, SLOT(shots()));
    connect(ui_shotsButton,SIGNAL(clicked()), this, SLOT(shots()));

    connect(m_actionAssets,SIGNAL(triggered(bool)), this, SLOT(assets()));
    connect(ui_assetsButton,SIGNAL(clicked()), this, SLOT(assets()));

    connect(m_actionSchedule,SIGNAL(triggered(bool)), this, SLOT(schedule()));
    connect(ui_scheduleButton, SIGNAL(clicked()), this, SLOT(schedule()));

    // Docks
    connect(m_projectAction, SIGNAL(triggered(bool)), ui_projectDockWidget, SLOT(setVisible(bool)));
    connect(ui_projectDockWidget, SIGNAL(visibilityChanged(bool)), m_projectAction, SLOT(setChecked(bool)));

    connect(m_stepsAction, SIGNAL(triggered(bool)), ui_stepsDockWidget, SLOT(setVisible(bool)));
    connect(ui_stepsDockWidget, SIGNAL(visibilityChanged(bool)), m_stepsAction, SLOT(setChecked(bool)));

    connect(m_pipeFilesAction, SIGNAL(triggered(bool)), ui_pipeFileDockWidget, SLOT(setVisible(bool)));
    connect(ui_pipeFileDockWidget, SIGNAL(visibilityChanged(bool)), m_pipeFilesAction, SLOT(setChecked(bool)));

    connect(m_assetGroupAction, SIGNAL(triggered(bool)), ui_assetGroupsDockWidget, SLOT(setVisible(bool)));
    connect(ui_assetGroupsDockWidget, SIGNAL(visibilityChanged(bool)), m_assetGroupAction, SLOT(setChecked(bool)));

    connect(m_assetListAction, SIGNAL(triggered(bool)), ui_assetsDockWidget, SLOT(setVisible(bool)));
    connect(ui_assetsDockWidget, SIGNAL(visibilityChanged(bool)), m_assetListAction, SLOT(setChecked(bool)));

    connect(m_sequenceAction, SIGNAL(triggered(bool)), ui_sequencesDockWidget, SLOT(setVisible(bool)));
    connect(ui_sequencesDockWidget, SIGNAL(visibilityChanged(bool)), m_sequenceAction, SLOT(setChecked(bool)));

    connect(m_shotListAction, SIGNAL(triggered(bool)), ui_shotsDockWidget, SLOT(setVisible(bool)));
    connect(ui_shotsDockWidget, SIGNAL(visibilityChanged(bool)), m_shotListAction, SLOT(setChecked(bool)));

    connect(m_actionStatistics,SIGNAL(triggered(bool)), ui_statsDockWidget, SLOT(setVisible(bool)));
    connect(ui_statsDockWidget,SIGNAL(visibilityChanged(bool)), m_actionStatistics, SLOT(setChecked(bool)));

    connect(ui_consoleDockWidget,&DuDockWidget::visibilityChanged, m_actionConsole, &DuAction::setChecked);
    connect(m_actionConsole, &DuAction::triggered, ui_consoleDockWidget, &DuDockWidget::setVisible);

    connect(m_actionTimeline,SIGNAL(triggered(bool)), ui_timelineDockWidget, SLOT(setVisible(bool)));
    connect(ui_timelineDockWidget,SIGNAL(visibilityChanged(bool)), m_actionTimeline, SLOT(setChecked(bool)));

    connect(m_actionSettings, &DuAction::triggered, ui_settingsDock, &DuDockWidget::setVisible);
    connect(ui_settingsDock, &DuDockWidget::visibilityChanged, m_actionSettings, &DuAction::setChecked);

    // Files
    connect(m_fileAdminAction,SIGNAL(triggered()), this, SLOT(revealAdminFolder()));
    connect(m_filePreprodAction,SIGNAL(triggered()), this, SLOT(revealPreProdFolder()));
    connect(m_fileProdAction,SIGNAL(triggered()), this, SLOT(revealProdFolder()));
    connect(m_filePostProdAction,SIGNAL(triggered()), this, SLOT(revealPostProdFolder()));
    connect(m_fileAssetsAction,SIGNAL(triggered()), this, SLOT(revealAssetsFolder()));
    connect(m_fileShotsAction,SIGNAL(triggered()), this, SLOT(revealShotsFolder()));
    connect(m_fileOutputAction,SIGNAL(triggered()), this, SLOT(revealOutputFolder()));
    connect(m_fileUserAction,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(m_fileVersionsAction,SIGNAL(triggered()), this, SLOT(revealVersionsFolder()));
    connect(m_fileTrashAction,SIGNAL(triggered()), this, SLOT(revealTrashFolder()));

    // Pages
    connect(ui_adminPage, SIGNAL(closeRequested()), this, SLOT(home()));

    // Other buttons
    connect(m_actionSync, SIGNAL(triggered()), DBInterface::instance(),SLOT(sync()));
    connect(m_actionFullSync, SIGNAL(triggered()), DBInterface::instance(),SLOT(fullSync()));
    connect(ui_mainStack,SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));

    // Misc
    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::raise);
    connect(Ramses::instance(),&Ramses::userChanged, this, &MainWindow::currentUserChanged);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &MainWindow::currentProjectChanged);
    connect(DBInterface::instance(),&DBInterface::connectionStatusChanged, this, &MainWindow::dbiConnectionStatusChanged);
    connect(DBInterface::instance(), &DBInterface::syncFinished, this, &MainWindow::finishSync);
    connect(DBInterface::instance(), &DBInterface::syncStarted, this, &MainWindow::startSync);

    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onQuit);
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

void MainWindow::raise()
{
    qDebug() << "Hello!";
    this->show();
    this->setWindowState( (windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    this->raise();  // for MacOS
    this->activateWindow(); // for Windows
}

void MainWindow::bugReport()
{
    QDesktopServices::openUrl ( QUrl( URL_BUGREPORT ) );
}

void MainWindow::forum()
{
    QDesktopServices::openUrl ( QUrl( URL_FORUM ) );
}

void MainWindow::chat()
{
    QDesktopServices::openUrl ( QUrl( URL_CHAT ) );
}

void MainWindow::doc()
{
    QDesktopServices::openUrl ( QUrl( URL_DOC ) );
}

void MainWindow::donate()
{
    QDesktopServices::openUrl ( QUrl( URL_DONATION ) );
}

void MainWindow::about()
{
    //duqf_aboutDialog->show();
}

void MainWindow::trayClicked(QSystemTrayIcon::ActivationReason reason)
{

    if (reason == QSystemTrayIcon::Trigger)
    {
        showHide();
    }
}

void MainWindow::showHide()
{
    if (this->isVisible())
    {
        m_actionShowHide->setIcon(DuIcon(":/icons/show-dark"));
        m_actionShowHide->setText("Show " + QString(STR_INTERNALNAME));
        emit hideTriggered();
        this->hide();
    }
    else
    {
        m_actionShowHide->setIcon(DuIcon(":/icons/hide-dark"));
        m_actionShowHide->setText("Hide " + QString(STR_INTERNALNAME));
        emit showTriggered();
        this->show();
    }
}

void MainWindow::askBeforeClose()
{
    QMessageBox::StandardButton r = QMessageBox::question(this, "Quitting Ramses", "Are you sure you want to quit Ramses?");
    if (r == QMessageBox::Yes) this->close();
}

void MainWindow::maximize(bool m)
{
    if (m) showMaximized();
    else showNormal();
}

void MainWindow::pageChanged(int i)
{
    m_actionAdmin->setChecked(i == 3);
    m_actionLogIn->setChecked(i == 0);
    ui_propertiesDockWidget->hide();
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
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(Home);
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

void MainWindow::admin()
{
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(Admin);
}

void MainWindow::pipeline()
{
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(PipeLine);
}

void MainWindow::shots()
{
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(Shots);
}

void MainWindow::assets()
{
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(Assets);
}

void MainWindow::schedule()
{
    ui_mainToolBar->show();
    ui_mainStack->setCurrentIndex(Schedule);
}

void MainWindow::currentUserChanged()
{
    disconnect(_currentUserConnection);

    //defaults
    ui_userButton->setText(QString("User (%1)").arg("Guest"));
    ui_userButton->setIcon(DuIcon(":/icons/user"));
    m_actionAdmin->setVisible(false);
    m_actionUserProfile->setVisible(false);
    m_actionUserFolder->setVisible(false);

    RamUser *user = Ramses::instance()->currentUser();
    if (!user)
    {
        m_actionLogIn->setVisible(true);
        m_actionLogOut->setVisible(false);
        ui_databaseButton->setVisible(false);
        ui_projectSelectorAction->setVisible(false);
        ui_userMenuAction->setVisible(false);
        ui_userWidget->setObject(nullptr);
        ui_userWidget->setEnabled(false);
        ui_userDockWidget->hide();
        home();
        return;
    }

    m_actionLogIn->setVisible(false);
    m_actionLogOut->setVisible(true);
    ui_databaseButton->setVisible(true);
    ui_projectSelectorAction->setVisible(true);
    ui_userMenuAction->setVisible(true);
    ui_userWidget->setObject(user);
    ui_userWidget->setEnabled(true);

    _currentUserConnection = connect(user, &RamUser::dataChanged, this, &MainWindow::currentUserChanged);

    ui_userButton->setText(QString("User (%1)").arg(user->shortName()));
    m_actionUserProfile->setVisible(true);
    m_actionUserFolder->setVisible(true);

    if (user->role() == RamUser::Admin)
    {
        m_actionAdmin->setVisible(true);
        ui_userButton->setIcon(DuIcon(":/icons/admin"));
    }
    else if (user->role() == RamUser::ProjectAdmin)
    {
        ui_userButton->setIcon(DuIcon(":/icons/project-admin"));
    }
    else if (user->role() == RamUser::Lead)
    {
        ui_userButton->setIcon(DuIcon(":/icons/lead"));
    }
    else
    {
        ui_userButton->setIcon(DuIcon(":/icons/user"));
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
        m_currentPageIndex = ui_mainStack->currentIndex();
        ui_mainToolBar->hide();
        ui_mainStack->setCurrentIndex(Progress);
    }
    else
    {
        ui_mainStack->setCurrentIndex(m_currentPageIndex);
        ui_mainToolBar->show();
    }
    this->repaint();
}

void MainWindow::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    ui_databaseMenuAction->setVisible(true);

    QString address =  RamServerInterface::instance()->serverAddress();
    if (s == NetworkUtils::Online)
    {
        m_actionSync->setVisible(true);
        m_actionFullSync->setVisible(true);
        ui_databaseButton->setText(address);
        m_actionSetOnline->setVisible(false);
        m_actionSetOffline->setVisible(true);

        if (RamServerInterface::instance()->ssl())
        {
            ui_databaseButton->setIcon(DuIcon(":/icons/shield"));
            ui_databaseButton->setToolTip("Connected.");
        }
        else
        {
            ui_databaseButton->setIcon(DuIcon(":/icons/no-shield"));
            ui_databaseButton->setToolTip(tr("WARNING: Connection is not secured!"));
        }
    }
    else if (s == NetworkUtils::Connecting) ui_databaseButton->setText(tr("Connecting to %1").arg(address));
    else if (s == NetworkUtils::Offline)
    {
        m_actionSync->setVisible(false);
        m_actionFullSync->setVisible(false);
        ui_databaseButton->setText(tr("Offline"));
        ui_databaseButton->setIcon(DuIcon(":/icons/storage"));
        m_actionSetOnline->setVisible(true);
        m_actionSetOffline->setVisible(false);
    }
}

void MainWindow::finishSync()
{
    if (m_closing) {
        m_readyToClose = true;
        this->close();
        return;
    }

    m_actionSync->setEnabled(true);
    m_actionSync->setText(tr("Quick sync"));
    m_actionFullSync->setEnabled(true);
    m_actionFullSync->setText(tr("Full sync"));

    // Refresh all UI!
    this->update();
}

void MainWindow::startSync()
{
    m_actionSync->setEnabled(false);
    m_actionFullSync->setEnabled(false);
    QString t = tr("Syncing...");
    m_actionSync->setText(t);
    m_actionFullSync->setText(t);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->objectName() == "fundingBar")
    {
        if (event->type() == QEvent::MouseButtonPress) {
            donate();
        }
    }

    return DuMainWindow::eventFilter(obj, event);
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

void MainWindow::setupActions()
{
    m_actionConsole = new DuAction(this);
    m_actionConsole->setText(tr("Console"));
    m_actionConsole->setIcon(DuIcon(":/icons/bash"));
    m_actionConsole->setCheckable(true);

    m_actionLogIn = new DuAction(this);
    m_actionLogIn->setText(tr("Log in..."));
    m_actionLogIn->setToolTip(tr("Logs you in the Ramses database."));
    m_actionLogIn->setIcon(":/icons/login");

    m_actionLogOut = new DuAction(this);
    m_actionLogOut->setText(tr("Log out"));
    m_actionLogOut->setToolTip(tr("Logs you out of the Ramses database."));
    m_actionLogOut->setIcon(":/icons/logout");

    m_actionUserProfile = new DuAction(this);
    m_actionUserProfile->setText(tr("Profile"));
    m_actionUserProfile->setToolTip(tr("Edit your user profile."));
    m_actionUserProfile->setIcon(":/icons/user-settings");

    m_actionAdmin = new DuAction(this);
    m_actionAdmin->setText(tr("Admin"));
    m_actionAdmin->setToolTip(tr("Edit global database settings and templates."));
    m_actionAdmin->setIcon(":/icons/settings");

    m_actionUserFolder = new DuAction(this);
    m_actionUserFolder->setText(tr("User folder"));
    m_actionUserFolder->setToolTip(tr("Opens your personal folder."));
    m_actionUserFolder->setIcon(":/icons/folder");

    m_actionPipeline = new DuAction(this);
    m_actionPipeline->setText(tr("Pipeline"));
    m_actionPipeline->setToolTip(tr("Edit the project pipeline."));
    m_actionPipeline->setIcon(":/icons/steps-menu");

    m_actionAssets = new DuAction(this);
    m_actionAssets->setText(tr("Assets Table"));
    m_actionAssets->setToolTip(tr("Track the assets production."));
    m_actionAssets->setIcon(":/icons/assets-table");

    m_actionShots = new DuAction(this);
    m_actionShots->setText(tr("Shots Table"));
    m_actionShots->setToolTip(tr("Track the shots production."));
    m_actionShots->setIcon(":/icons/shots-table");

    m_actionSchedule = new DuAction(this);
    m_actionSchedule->setText(tr("Schedule"));
    m_actionSchedule->setToolTip(tr("Edit the user schedules."));
    m_actionSchedule->setIcon(":/icons/calendar");

    m_actionStatistics = new DuAction(this);
    m_actionStatistics->setText(tr("Statistics"));
    m_actionStatistics->setCheckable(true);
    m_actionStatistics->setToolTip(tr("Shows some project statics and production tracking data."));
    m_actionStatistics->setIcon(":/icons/stats");

    m_actionTimeline = new DuAction(this);
    m_actionTimeline->setText(tr("Timeline"));
    m_actionTimeline->setCheckable(true);
    m_actionTimeline->setToolTip(tr("Shows the shots in a timeline."));
    m_actionTimeline->setIcon(":/icons/timeline");

    m_actionSetOnline = new DuAction(this);
    m_actionSetOnline->setText(tr("Set online"));
    m_actionSetOnline->setToolTip(tr("Connects to the Ramses server."));
    m_actionSetOnline->setIcon(":/icons/server-settings");

    m_actionSetOffline = new DuAction(this);
    m_actionSetOffline->setText(tr("Set offline"));
    m_actionSetOffline->setToolTip(tr("Disconnects from the Ramses server and deactivates sync."));
    m_actionSetOffline->setIcon(":/icons/folder");

    m_actionDatabaseSettings = new DuAction(this);
    m_actionDatabaseSettings->setText(tr("Database settings"));
    m_actionDatabaseSettings->setToolTip(tr("Edit the database settings."));
    m_actionDatabaseSettings->setIcon(":/icons/storage-settings");

    m_actionSettings = new DuAction(this);
    m_actionSettings->setText(tr("Settings"));
    m_actionSettings->setToolTip(tr("Edit the application settings."));
    m_actionSettings->setCheckable(true);
    m_actionSettings->setIcon(":/icons/settings");

    m_actionSync = new DuAction(this);
    QList<QKeySequence> syncSC;
    syncSC << QKeySequence("Ctrl+R");
    syncSC << QKeySequence("F5");
    m_actionSync->setShortcuts( syncSC );
    m_actionSync->setText(tr("Quick sync"));
    m_actionSync->setToolTip(tr("Quickly syncs the most recent data with the server."));
    m_actionSync->setIcon(":/icons/check-update");

    m_actionFullSync = new DuAction(this);
    m_actionFullSync->setText(tr("Full sync"));
    m_actionFullSync->setToolTip(tr("Syncs all the data with the server."));
    m_actionFullSync->setIcon(":/icons/check-update");
    m_actionFullSync->setShortcut(QKeySequence("Ctrl+Shift+R"));

    m_projectAction = new DuAction(tr("Project settings"), this);
    m_projectAction->setIcon(":/icons/project");
    m_projectAction->setToolTip(tr("Edit the project settings."));
    m_projectAction->setCheckable(true);

    m_stepsAction = new DuAction(tr("Steps"), this);
    m_stepsAction->setIcon(":/icons/step");
    m_stepsAction->setToolTip(tr("Shows the list of steps."));
    m_stepsAction->setCheckable(true);

    m_pipeFilesAction = new DuAction(tr("Pipe formats"), this);
    m_pipeFilesAction->setIcon(":/icons/connection");
    m_pipeFilesAction->setToolTip(tr("Shows the list of pipes."));
    m_pipeFilesAction->setCheckable(true);

    m_assetGroupAction = new DuAction(tr("Asset groups"), this);
    m_assetGroupAction->setIcon(":/icons/asset-group");
    m_assetGroupAction->setToolTip(tr("Shows the list of asset groups."));
    m_assetGroupAction->setCheckable(true);

    m_assetListAction = new DuAction(tr("Assets"), this);
    m_assetListAction->setIcon(":/icons/asset");
    m_assetListAction->setToolTip(tr("Shows the list of assets."));
    m_assetListAction->setCheckable(true);

    m_sequenceAction = new DuAction(tr("Sequences"), this);
    m_sequenceAction->setIcon(":/icons/sequence");
    m_sequenceAction->setToolTip(tr("Shows the list of sequences."));
    m_sequenceAction->setCheckable(true);

    m_shotListAction = new DuAction(tr("Shots"), this);
    m_shotListAction->setIcon(":/icons/shot");
    m_shotListAction->setToolTip(tr("Shows the list of shots."));
    m_shotListAction->setCheckable(true);

    m_fileAdminAction = new DuAction(tr("Admin"), this);
    m_fileAdminAction->setIcon(":/icons/settings-w");

    m_filePreprodAction = new DuAction(tr("Pre-production"), this);
    m_filePreprodAction->setIcon(":/icons/project");

    m_fileProdAction = new DuAction(tr("Production"), this);
    m_fileProdAction->setIcon(":/icons/sequence");

    m_filePostProdAction = new DuAction(tr("Post-production"), this);
    m_filePostProdAction->setIcon(":/icons/film");

    m_fileAssetsAction = new DuAction(tr("Assets"), this);
    m_fileAssetsAction->setIcon(":/icons/asset");

    m_fileShotsAction = new DuAction(tr("Shots"), this);
    m_fileShotsAction->setIcon(":/icons/shot");

    m_fileOutputAction = new DuAction(tr("Output"), this);
    m_fileOutputAction->setIcon(":/icons/output-folder");

    m_fileUserAction = new DuAction(tr("My user folder"), this);
    m_fileUserAction->setIcon(":/icons/user");

    m_fileVersionsAction = new DuAction(tr("Project versions"), this);
    m_fileVersionsAction->setIcon(":/icons/versions-folder");

    m_fileTrashAction = new DuAction(tr("Project trash"), this);
    m_fileTrashAction->setIcon(":/icons/trash");
}

void MainWindow::setupUi()
{
    ui_mainStack = new QStackedWidget(this);
    this->setCentralWidget(ui_mainStack);

    ui_mainPage = new QWidget(ui_mainStack);
    ui_mainStack->addWidget(ui_mainPage);

    auto mainLayout = new QVBoxLayout(ui_mainPage);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    LoginPage *lp = new LoginPage(this);
    mainLayout->addWidget(lp);

    // admin
    ui_adminPage = new SettingsWidget("Administration", this);
    ui_adminPage->titleBar()->setObjectName("adminToolBar");
    ui_adminPage->showReinitButton(false);
    ui_mainStack->addWidget(ui_adminPage);
    // Admin tabs

    UserManagerWidget *userManager = new UserManagerWidget(this);
    ui_adminPage->addPage(userManager,"Users", DuIcon(":/icons/users"));
    ui_adminPage->titleBar()->insertLeft(userManager->menuButton());

    ProjectManagerWidget *projectManager = new ProjectManagerWidget(this);
    ui_adminPage->addPage(projectManager, "Projects", DuIcon(":/icons/projects"));
    ui_adminPage->titleBar()->insertLeft(projectManager->menuButton());

    TemplateStepManagerWidget *templateStepManager = new TemplateStepManagerWidget(this);
    ui_adminPage->addPage(templateStepManager, "Template Steps", DuIcon(":/icons/steps"));
    ui_adminPage->titleBar()->insertLeft(templateStepManager->menuButton());

    TemplateAssetGroupManagerWidget *templateAssetGroupManager = new TemplateAssetGroupManagerWidget(this);
    ui_adminPage->addPage(templateAssetGroupManager, "Template Asset Groups", DuIcon(":/icons/asset-groups"));
    ui_adminPage->titleBar()->insertLeft(templateAssetGroupManager->menuButton());

    StateManagerWidget *stateManager = new StateManagerWidget(this);
    ui_adminPage->addPage(stateManager, "States", DuIcon(":/icons/state"));
    ui_adminPage->titleBar()->insertLeft(stateManager->menuButton());

    FileTypeManagerWidget *fileTypeManager = new FileTypeManagerWidget(this);
    ui_adminPage->addPage(fileTypeManager, "File Types", DuIcon(":/icons/files"));
    ui_adminPage->titleBar()->insertLeft(fileTypeManager->menuButton());

    ApplicationManagerWidget *applicationManager = new ApplicationManagerWidget(this);
    ui_adminPage->addPage(applicationManager, "Applications", DuIcon(":/icons/applications"));
    ui_adminPage->titleBar()->insertLeft(applicationManager->menuButton());

    DBManagerWidget *dbManager = new DBManagerWidget(this);
    ui_adminPage->addPage(dbManager, "Database tools", DuIcon(":/icons/applications"));

// Pipeline editor
#ifndef DEACTIVATE_PIPELINE
    PipelineWidget *pipelineEditor = new PipelineWidget(this);
    ui_mainStack->addWidget(pipelineEditor);
    connect(pipelineEditor, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Pipeline ready";//*/
#endif

#ifndef DEACTIVATE_ASSETSTABLE
    ItemManagerWidget *assetsTable = new ItemManagerWidget(RamStep::AssetProduction, this);
    ui_mainStack->addWidget(assetsTable);
    connect(assetsTable, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Assets table ready";
#endif

#ifndef DEACTIVATE_SHOTSTABLE
    ItemManagerWidget *shotsTable = new ItemManagerWidget(RamStep::ShotProduction, this);
    ui_mainStack->addWidget(shotsTable);
    connect(shotsTable, SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Shots table ready";
#endif

#ifndef DEACTIVATE_SCHEDULE
    ScheduleManagerWidget *scheduleTable = new ScheduleManagerWidget(this);
    ui_mainStack->addWidget(scheduleTable);
    connect(scheduleTable,SIGNAL(closeRequested()), this, SLOT(home()));
    qDebug() << "> Schedule ready";
#endif

    // Progress page
    progressPage = new ProgressPage(this);
    ui_mainStack->addWidget(progressPage);
}

void MainWindow::setupDocks()
{
    QSettings settings;

    // Settings Dock
    ui_settingsDock = new DuDockWidget("Settings", this);
    ui_settingsDock->setWindowIcon(DuIcon(":/icons/settings"));
    ui_settingsDock->setObjectName("settingsDock");
    DuDockTitleWidget *ui_settingsTitle = new DuDockTitleWidget("Settings", this);
    ui_settingsTitle->setObjectName("dockTitle");
    ui_settingsTitle->setIcon(":/icons/settings");
    ui_settingsDock->setTitleBarWidget(ui_settingsTitle);
    ui_settingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_settingsDock->setFeatures(DuDockWidget::DockWidgetClosable | DuDockWidget::DockWidgetMovable | DuDockWidget::DockWidgetFloatable );
    ui_settingsDock->setWidget(new SettingsDock(ui_settingsDock));
    this->addDockWidget(Qt::RightDockWidgetArea, ui_settingsDock);
    ui_settingsDock->hide();

#ifndef DEACTIVATE_STATS
    StatisticsWidget *statsTable = new StatisticsWidget(this);

    ui_statsDockWidget = new DuDockWidget(tr("Statistics"));
    ui_statsDockWidget->setWindowIcon(DuIcon(":/icons/stats"));
    ui_statsDockWidget->setObjectName("statsDock");

    ui_statsTitle = new DuDockTitleWidget(tr("Statistics"), this);
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

    ui_consoleDockWidget = new DuDockWidget("Console");
    ui_consoleDockWidget->setWindowIcon(DuIcon(":/icons/bash"));
    ui_consoleDockWidget->setObjectName("consoleDock");

    auto *consoleTitle = new DuDockTitleWidget("Console", this);
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

    ui_timelineDockWidget = new DuDockWidget("Timeline");
    ui_timelineDockWidget->setWindowIcon(DuIcon(":/icons/timeline"));
    ui_timelineDockWidget->setObjectName("timelineDock");

    auto *timelineTitle = new DuDockTitleWidget("Timeline", this);
    timelineTitle->setObjectName("dockTitle");
    timelineTitle->setIcon(":/icons/timeline");
    ui_timelineDockWidget->setTitleBarWidget(timelineTitle);

    ui_timelineDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    ui_timelineDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_timelineDockWidget->setWidget( timeline );
    this->addDockWidget(Qt::BottomDockWidgetArea, ui_timelineDockWidget);
    ui_timelineDockWidget->hide();

    qDebug() << "> Timeline dock ready";

    // The properties dock
    ui_propertiesDockWidget = new DuDockWidget("Properties");
    ui_propertiesDockWidget->setWindowIcon(DuIcon(":/icons/asset"));
    ui_propertiesDockWidget->setObjectName("propertiesDock");

    ui_propertiesTitle = new DuDockTitleWidget("Properties", this);
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

    ui_projectDockWidget = new DuDockWidget(tr("Project settings"));
    ui_projectDockWidget->setWindowIcon(DuIcon(":/icons/project"));
    ui_projectDockWidget->setObjectName("projectDock");

    auto *pTitle = new DuDockTitleWidget(tr("Project settings"), this);
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

    ui_stepsDockWidget = new DuDockWidget(tr("Steps"));
    ui_stepsDockWidget->setWindowIcon(DuIcon(":/icons/step"));
    ui_stepsDockWidget->setObjectName("stepsDock");

    auto *sTitle = new DuDockTitleWidget(tr("Steps"), this);
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

    ui_pipeFileDockWidget = new DuDockWidget(tr("Pipe formats"));
    ui_pipeFileDockWidget->setWindowIcon(DuIcon(":/icons/connection"));
    ui_pipeFileDockWidget->setObjectName("pipeFilesDock");

    auto *pfTitle = new DuDockTitleWidget(tr("Pipe formats"), this);
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

    ui_assetGroupsDockWidget = new DuDockWidget(tr("Asset Groups"));
    ui_assetGroupsDockWidget->setWindowIcon(DuIcon(":/icons/asset-group"));
    ui_assetGroupsDockWidget->setObjectName("assetGroupsDock");

    auto *agTitle = new DuDockTitleWidget(tr("Asset Groups"), this);
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

    ui_assetsDockWidget = new DuDockWidget(tr("Assets"));
    ui_assetsDockWidget->setWindowIcon(DuIcon(":/icons/asset"));
    ui_assetsDockWidget->setObjectName("assetsDock");

    auto *aTitle = new DuDockTitleWidget(tr("Assets"), this);
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

    ui_sequencesDockWidget = new DuDockWidget(tr("Sequences"));
    ui_sequencesDockWidget->setWindowIcon(DuIcon(":/icons/sequence"));
    ui_sequencesDockWidget->setObjectName("sequencesDock");

    auto *seqTitle = new DuDockTitleWidget(tr("Sequences"), this);
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

    ui_shotsDockWidget = new DuDockWidget(tr("Shots"));
    ui_shotsDockWidget->setWindowIcon(DuIcon(":/icons/shot"));
    ui_shotsDockWidget->setObjectName("shotsDock");

    auto *shTitle = new DuDockTitleWidget(tr("Shots"), this);
    shTitle->setObjectName("dockTitle");
    shTitle->setIcon(":/icons/shot");
    ui_shotsDockWidget->setTitleBarWidget(shTitle);

    ui_shotsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_shotsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_shotsDockWidget->setWidget( shotWidget );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_shotsDockWidget);
    ui_shotsDockWidget->hide();

    // The User Profile dock
    ui_userWidget = new UserEditWidget(this);

    ui_userDockWidget = new DuDockWidget(tr("User profile"));
    ui_userDockWidget->setWindowIcon(DuIcon(":/icons/user"));
    ui_userDockWidget->setObjectName("userDock");

    auto *userTitle = new DuDockTitleWidget(tr("User profile"), this);
    userTitle->setObjectName("dockTitle");
    userTitle->setIcon(":/icons/user");
    ui_userDockWidget->setTitleBarWidget(userTitle);

    ui_userDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_userDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_userDockWidget->setWidget( ui_userWidget );
    this->addDockWidget(Qt::RightDockWidgetArea, ui_userDockWidget);
    ui_userDockWidget->hide();
}

void MainWindow::setupToolBar()
{
    ui_mainToolBar->addAction(m_actionAdmin);

    DuMenu *pipelineMenu = new DuMenu(this);

    pipelineMenu->addAction(m_actionPipeline);
    pipelineMenu->addSeparator();
    pipelineMenu->addAction(m_projectAction);
    pipelineMenu->addAction(m_stepsAction);
    pipelineMenu->addAction(m_pipeFilesAction);

    ui_pipelineButton = new QToolButton();
    ui_pipelineButton->setIcon(DuIcon(":icons/steps-menu"));
    ui_pipelineButton->setText("Pipeline");
    ui_pipelineButton->setMenu(pipelineMenu);
    ui_pipelineButton->setIconSize(QSize(16,16));
    ui_pipelineButton->setPopupMode(QToolButton::InstantPopup);
    ui_pipelineButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    DuMenu *assetsMenu = new DuMenu(this);

    assetsMenu->addAction(m_actionAssets);
    assetsMenu->addSeparator();
    assetsMenu->addAction(m_assetGroupAction);
    assetsMenu->addAction(m_assetListAction);

    ui_assetsButton = new QToolButton();
    ui_assetsButton->setIcon(DuIcon(":/icons/asset"));
    ui_assetsButton->setText("Assets");
    ui_assetsButton->setMenu(assetsMenu);
    ui_assetsButton->setIconSize(QSize(16,16));
    ui_assetsButton->setPopupMode(QToolButton::InstantPopup);
    ui_assetsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    DuMenu *shotsMenu = new DuMenu(this);

    shotsMenu->addAction(m_actionShots);
    shotsMenu->addSeparator();
    shotsMenu->addAction(m_sequenceAction);
    shotsMenu->addAction(m_shotListAction);
    shotsMenu->addAction(m_actionTimeline);

    ui_shotsButton = new QToolButton();
    ui_shotsButton->setIcon(DuIcon(":/icons/shot"));
    ui_shotsButton->setText("Shots");
    ui_shotsButton->setMenu(shotsMenu);
    ui_shotsButton->setIconSize(QSize(16,16));
    ui_shotsButton->setPopupMode(QToolButton::InstantPopup);
    ui_shotsButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    DuMenu *scheduleMenu = new DuMenu(this);

    scheduleMenu->addAction(m_actionSchedule);
    scheduleMenu->addSeparator();
    scheduleMenu->addAction(m_actionStatistics);

    ui_scheduleButton = new QToolButton();
    ui_scheduleButton->setIcon(DuIcon(":/icons/calendar"));
    ui_scheduleButton->setText("Schedule");
    ui_scheduleButton->setMenu(scheduleMenu);
    ui_scheduleButton->setIconSize(QSize(16,16));
    ui_scheduleButton->setPopupMode(QToolButton::InstantPopup);
    ui_scheduleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    DuMenu *filesMenu = new DuMenu(this);

    filesMenu->addAction(m_fileAdminAction);
    filesMenu->addAction(m_filePreprodAction);
    filesMenu->addAction(m_fileProdAction);
    filesMenu->addAction(m_filePostProdAction);
    filesMenu->addAction(m_fileAssetsAction);
    filesMenu->addAction(m_fileShotsAction);
    filesMenu->addAction(m_fileOutputAction);
    filesMenu->addSeparator();
    filesMenu->addAction(m_fileUserAction);
    filesMenu->addSeparator();
    filesMenu->addAction(m_fileVersionsAction);
    filesMenu->addAction(m_fileTrashAction);

    ui_filesButton = new QToolButton();
    ui_filesButton->setIcon(DuIcon(":/icons/folder"));
    ui_filesButton->setText("Files");
    ui_filesButton->setMenu(filesMenu);
    ui_filesButton->setIconSize(QSize(16,16));
    ui_filesButton->setPopupMode(QToolButton::InstantPopup);
    ui_filesButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    // Populate Toolbar
    auto projectSelector = new ProjectSelectorWidget(this);
    ui_projectSelectorAction = ui_mainToolBar->addWidget(projectSelector);
    DuUI::addCustomCSS(projectSelector, "QComboBox { background: #222222; }");

    ui_pipelineMenuAction = ui_mainToolBar->addWidget(ui_pipelineButton);
    ui_assetMenuAction = ui_mainToolBar->addWidget(ui_assetsButton);
    ui_shotMenuAction = ui_mainToolBar->addWidget(ui_shotsButton);
    ui_scheduleMenuAction = ui_mainToolBar->addWidget(ui_scheduleButton);
    ui_filesMenuAction = ui_mainToolBar->addWidget(ui_filesButton);

    auto logB = new DuQFLogToolButton(this);
    QAction *logA = ui_mainToolBar->addWidget(logB);
    connect(logB, &DuQFLogToolButton::visibilityChanged, logA, &QAction::setVisible);

    ui_projectSelectorAction->setVisible(false);
    m_actionAdmin->setVisible(false);
    ui_pipelineMenuAction->setVisible(false);
    ui_shotMenuAction->setVisible(false);
    ui_assetMenuAction->setVisible(false);
    ui_scheduleMenuAction->setVisible(false);
    ui_filesMenuAction->setVisible(false);

    ui_mainToolBar->addWidget(new DuToolBarSpacer());

    auto userMenu = new DuMenu();
    userMenu->addAction(m_actionLogIn);
    userMenu->addAction(m_actionUserFolder);
    m_actionUserFolder->setVisible(false);
    userMenu->addAction(m_actionUserProfile);
    m_actionUserProfile->setVisible(false);
    userMenu->addAction(m_actionLogOut);
    m_actionLogOut->setVisible(false);

    ui_userButton = new QToolButton();
    ui_userButton->setText(QString("User (%1)").arg("Guest"));
    ui_userButton->setIcon(DuIcon(":/icons/user"));
    ui_userButton->setPopupMode(QToolButton::InstantPopup);
    ui_userButton->setIconSize(QSize(16,16));
    ui_userButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_userButton->setMenu(userMenu);

    ui_userMenuAction = ui_mainToolBar->addWidget(ui_userButton);
    ui_userMenuAction->setVisible(false);

    auto databaseMenu = new DuMenu();
    databaseMenu->addAction(m_actionSync);
    databaseMenu->addAction(m_actionFullSync);
    databaseMenu->addSeparator();
    databaseMenu->addAction(m_actionSetOffline);
    databaseMenu->addAction(m_actionSetOnline);
    databaseMenu->addAction(m_actionDatabaseSettings);
    m_actionSetOffline->setVisible(false);

    ui_databaseButton = new QToolButton();
    ui_databaseButton->setText(tr("Offline"));
    ui_databaseButton->setIcon(DuIcon(":/icons/storage"));
    ui_databaseButton->setPopupMode(QToolButton::InstantPopup);
    ui_databaseButton->setIconSize(QSize(16,16));
    ui_databaseButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    ui_databaseButton->setMenu(databaseMenu);

    ui_databaseMenuAction = ui_mainToolBar->addWidget(ui_databaseButton);
    ui_databaseMenuAction->setVisible(false);

    DuMenu *moreMenu = new DuMenu(this);

    QToolButton *moreButton = new QToolButton();
    moreButton->setText(tr("More"));
    moreButton->setIcon(DuIcon(":/icons/more"));
    moreButton->setPopupMode(QToolButton::InstantPopup);
    moreButton->setIconSize(QSize(16,16));
    moreButton->setMenu(moreMenu);
    moreButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    moreMenu->addSeparator();
    moreMenu->addAction(m_actionConsole);
    moreMenu->addAction(m_actionSettings);

    auto helpMenu = new DuMenu(this);
    helpMenu->setIcon(DuIcon(":/icons/help"));
    helpMenu->setTitle(tr("Help"));
    moreMenu->addSeparator();
    moreMenu->addMenu(helpMenu);

    if (QString(URL_DOC) != "")
    {
        DuAction *docAction = new DuAction("Help", this);
        docAction->setIcon(":/icons/documentation");
        docAction->setToolTip("Read the documentation");
        docAction->setShortcut(QKeySequence("F1"));
        helpMenu->addAction(docAction);
        connect(docAction, &DuAction::triggered, this, &MainWindow::doc);
    }
    DuAction *aboutAction = new DuAction(tr("About"), this);
    aboutAction->setIcon(":/icons/info");
    helpMenu->addAction(aboutAction);
    connect(aboutAction, &DuAction::triggered, this, &MainWindow::about);
    helpMenu->addSeparator();
    bool chat = QString(URL_CHAT) != "";
    bool bugReport = QString(URL_BUGREPORT) != "";
    bool forum = QString(URL_FORUM) != "";
    bool donate = QString(URL_DONATION) != "";
    if (bugReport)
    {
        DuAction *bugReportAction = new DuAction(tr("Bug Report"), this);
        bugReportAction->setIcon(":/icons/bug-report");
        bugReportAction->setToolTip("Report a DuIcon");
        helpMenu->addAction(bugReportAction);
        if (!chat && !forum && !donate) helpMenu->addSeparator();
        connect(bugReportAction, &DuAction::triggered, this, &MainWindow::bugReport);
    }
    if (chat)
    {
        DuAction *chatAction = new DuAction(tr("Chat"), this);
        chatAction->setIcon(":/icons/chat");
        chatAction->setToolTip("Come and have a chat");
        helpMenu->addAction(chatAction);
        if (!forum && !donate) helpMenu->addSeparator();
        connect(chatAction, &DuAction::triggered, this, &MainWindow::chat);
    }
    if (forum)
    {
        DuAction *forumAction = new DuAction(tr("Forum"), this);
        forumAction->setIcon(":/icons/forum");
        forumAction->setToolTip("Join us on our forum");
        helpMenu->addAction(forumAction);
        if (!donate) helpMenu->addSeparator();
        connect(forumAction, SIGNAL(triggered()), this, SLOT(duqf_forum()));
    }
    if (donate)
    {
        DuAction *donateAction = new DuAction("I ♥ " + QString(STR_FILEDESCRIPTION), this);
        donateAction->setIcon(":/icons/donate");
        donateAction->setToolTip("Help us, donate now!");
        helpMenu->addAction(donateAction);
        helpMenu->addSeparator();
        moreMenu->addAction(donateAction);
        connect(donateAction, &DuAction::triggered, this, &MainWindow::donate);
    }
    DuAction *aboutQtAction = new DuAction(tr("About Qt"), this);
    aboutQtAction->setIcon(":/icons/qt");
    helpMenu->addAction(aboutQtAction);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    ui_mainToolBar->addWidget(moreButton);
}

void MainWindow::setupSysTray()
{
    m_actionShowHide = new DuAction("Hide " + QString(STR_INTERNALNAME), this);
    m_actionShowHide->setIcon(":/icons/hide-dark");
    bool useSysTray = QSystemTrayIcon::isSystemTrayAvailable() && USE_SYSTRAY;
    if (useSysTray)
    {
        DuMenu *trayMenu = new DuMenu(QString(STR_INTERNALNAME),this);
        QSettings settings;

#ifdef Q_OS_LINUX
        QString trayIconType = settings.value("appearance/trayIconType", "light").toString();
#else
        QString trayIconType = settings.value("appearance/trayIconType", "color").toString();
#endif
        trayIcon = new QSystemTrayIcon(DuIcon(":/icons/tray-" + trayIconType),this);
        trayMenu->addAction(m_actionShowHide);
        DuAction *actionQuit = new DuAction("Quit", this);
        actionQuit->setIcon(":/icons/close-dark");
        trayMenu->addAction(actionQuit);
        trayIcon->setContextMenu(trayMenu);
        trayIcon->show();
        connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
        connect(m_actionShowHide, &QAction::triggered, this, &MainWindow::showHide);
        connect(actionQuit, &QAction::triggered, this, &MainWindow::close);
    }
}
