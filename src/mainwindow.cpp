#include "mainwindow.h"

#include <QFontDatabase>

#include "assetgroupmanagerwidget.h"
#include "assetmanagerwidget.h"
#include "docks/consoledock.h"
#include "homepage.h"
#include "itemmanagerwidget.h"
#include "pipefilemanagerwidget.h"
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
#include "dbmanagerwidget.h"
#include "duapp/app-version.h"
#include "docks/settingsdock.h"
#include "ramses.h"
#include "duapp/duui.h"

MainWindow::MainWindow(const QCommandLineParser &cli, QWidget *parent) :
    DuMainWindow{parent}
{
    qDebug() << "> Initializing threads and workers";

    // We instantiate all these objects to be sure it's done in the right order.

    // The database interface
    DBInterface::i();
    // The State manager
    StateManager::i();
    // Ramses
    Ramses::i();

    qDebug() << "> Loading settings";

    QSettings settings;

    qDebug() << "> Setting up menus";

    // Setup the actions
    setupActions();
    // Build the window
    setupUi();
    // Build the toolbars
    setupToolBars();
    // Build the sys tray icon
    setupSysTray();
    // Build the docks
    setupDocks();

    // Set style
    setStyle();

    // Initial page
    setPage(Landing);

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
    const QStringList args = cli.positionalArguments();
    for(const QString &filePath: args) {
        QFileInfo argInfo(filePath);
        if (argInfo.exists() && argInfo.suffix().toLower() == "ramses")
        {
            DBInterface::i()->loadDataFile(filePath);
            break;
        }
    }
    //*/
}

void MainWindow::connectEvents()
{
    // Connect events

    // ==== Toolbar buttons ====

    connect(m_actionLogIn,&DuAction::triggered,
            this, [this] () { setPage(Landing); } );
    connect(m_actionLogOut, &QAction::triggered, this, []() { StateManager::i()->restart(); });
    connect(m_actionSetOnline, &QAction::triggered, this, &MainWindow::setOnlineAction);
    connect(m_actionSetOffline, &QAction::triggered, this, &MainWindow::setOfflineAction);
    connect(m_actionDatabaseSettings, &QAction::triggered, this, &MainWindow::databaseSettingsAction);
    connect(m_actionUserFolder,SIGNAL(triggered()), this, SLOT(revealUserFolder()));

    connect(m_actionUserProfile,&QAction::triggered,
            this, [] () {
        RamUser *user = Ramses::i()->currentUser();
        if (user)
            user->edit(true);
    });

    connect(m_actionAdmin,&DuAction::triggered,
            this, [this] () { setPage(Admin); } );

    connect(m_actionPipeline,&DuAction::triggered,
            this, [this] () { setPage(PipeLine); } );
    connect(ui_pipelineButton,&QToolButton::triggered,
            this, [this] () { setPage(PipeLine); } );

    connect(m_actionShots,&DuAction::triggered,
            this, [this] () { setPage(Shots); } );
    connect(ui_shotsButton,&QToolButton::triggered,
            this, [this] () { setPage(Shots); } );

    connect(m_actionAssets,&DuAction::triggered,
            this, [this] () { setPage(Assets); } );
    connect(ui_assetsButton,&QToolButton::triggered,
            this, [this] () { setPage(Assets); } );

    connect(m_actionSchedule,&DuAction::triggered,
            this, [this] () { setPage(Schedule); } );
    connect(ui_scheduleButton,&QToolButton::triggered,
            this, [this] () { setPage(Schedule); } );

    // ==== Docks ====

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

    // ==== Files ====

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

    // ==== Other buttons ====

    connect(m_actionSync, SIGNAL(triggered()), DBInterface::i(),SLOT(sync()));
    connect(m_actionFullSync, SIGNAL(triggered()), DBInterface::i(),SLOT(fullSync()));
    connect(ui_mainStack,SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));

    // ==== State ====

    connect(StateManager::i(), &StateManager::stateChanged, this, &MainWindow::changeState);

    connect(&_ioTimer, &QTimer::timeout, this, [this] () {
        ui_ioWidget->setSVGIcon("");
        _ioTimer.stop();
    });

    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::raise);

    connect(Ramses::i(), &Ramses::ready, this, &MainWindow::ramsesReady);
    connect(Ramses::i(), &Ramses::roleChanged, this, &MainWindow::changeUserRole);

    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onQuit);
}

void MainWindow::setPropertiesDockWidget(QWidget *w, QString title, QString icon, bool temporary)
{
    if (_propertiesWidgetIsTemporary) {
        QWidget *w = ui_propertiesDockWidget->widget();
        if (w) {
            disconnect(w, nullptr, ui_propertiesDockWidget, nullptr);
            w->deleteLater();
        }
    }

    connect(w, &QWidget::destroyed, ui_propertiesDockWidget, &QDockWidget::hide);

    _propertiesWidgetIsTemporary = temporary;
    ui_propertiesDockWidget->setWidget( w );
    ui_propertiesTitle->setTitle(title);
    ui_propertiesTitle->setIcon(icon);
    ui_propertiesDockWidget->show();
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
    DuMainWindow::raise();  // for MacOS
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
    //DBInterface::i()->setOffline();
}

void MainWindow::setOnlineAction()
{
    /*DBInterface::i()->setOnline();

    // Trigger a full sync
    if (RamServerInterface::instance()->isOnline()) DBInterface::i()->fullSync();*/
}

void MainWindow::databaseSettingsAction()
{
    QString dataFilePath = DBInterface::i()->dataFile();
    if (dataFilePath == "") return;

    QFileInfo dataFile(dataFilePath);
    if (!dataFile.exists()) return;
}

void MainWindow::setPage(Page p)
{
    // If the UI is frozen, delay until freezeUI(false) is called
    /*if (ui_mainStack->currentIndex() == Progress)
        m_currentPageIndex = p;
    // Else sit it right now
    else*/
        ui_mainStack->setCurrentIndex(p);
}

void MainWindow::revealAdminFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::AdminFolder), true );
}

void MainWindow::revealPreProdFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::PreProdFolder), true );
}

void MainWindow::revealProdFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::ProdFolder), true );
}

void MainWindow::revealPostProdFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::PostProdFolder), true );
}

void MainWindow::revealAssetsFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::AssetsFolder), true );
}

void MainWindow::revealShotsFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::ShotsFolder), true );
}

void MainWindow::revealOutputFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::OutputFolder), true );
}

void MainWindow::revealUserFolder()
{
    RamUser *current = Ramses::i()->currentUser();
    if (current) FileUtils::openInExplorer( current->path(), true );
}

void MainWindow::revealVersionsFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::VersionsFolder), true );
}

void MainWindow::revealTrashFolder()
{
    RamProject *proj = Ramses::i()->project();
    if (!proj) return;
    FileUtils::openInExplorer( proj->path(RamProject::TrashFolder), true );
}

void MainWindow::changeUserRole(RamAbstractObject::UserRole role)
{
    switch(role) {
    case RamAbstractObject::Admin:
        m_actionAdmin->setVisible(true);
        break;
    case RamAbstractObject::ProjectAdmin:
    case RamAbstractObject::Lead:
    case RamAbstractObject::Standard:
        m_actionAdmin->setVisible(false);
        break;
    }
}

void MainWindow::ramsesReady()
{
    // Set user

    // Guaranted to exist,
    // The signal is emited only if all are ready
    RamUser *user = Ramses::i()->currentUser();

    ui_userButton->setText(QString("User (%1)").arg(user->shortName()));
    m_actionUserProfile->setVisible(true);
    m_actionUserFolder->setVisible(true);
    m_actionLogIn->setVisible(false);
    m_actionLogOut->setVisible(true);
    ui_databaseButton->setVisible(true);
    ui_userMenuAction->setVisible(true);

    // Set Project

    // Guaranted to exist,
    // The signal is emited only if all are ready
    RamProject *project = Ramses::i()->project();

    ui_projectEditWiget->setObject(project);
    ui_statsTitle->setTitle( project->name() );

    ui_shotMenuAction->setVisible(true);
    ui_assetMenuAction->setVisible(true);
    ui_scheduleMenuAction->setVisible(true);
    ui_filesMenuAction->setVisible(true);

    ui_pipelineMenuAction->setVisible(
        Ramses::i()->isProjectAdmin()
        );

    setWindowTitle(project->name());

    updateWindow();
}

void MainWindow::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    /*ui_databaseMenuAction->setVisible(true);

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
    }*/
}

void MainWindow::changeState(StateManager::State s)
{
    // Set the read/write icon
    switch(s) {
    case StateManager::WritingDataBase:
        ui_ioWidget->setSVGIcon(":/icons/database-write");
        _ioTimer.start(_minIOInterval);
        break;
    case StateManager::LoadingDataBase:
        ui_ioWidget->setSVGIcon(":/icons/database-read");
        _ioTimer.start(_minIOInterval);
        break;

    case StateManager::Unknown:
    case StateManager::Idle:
    case StateManager::Opening:
    case StateManager::Connecting:
    case StateManager::Closing:
    case StateManager::Syncing:
        break;
    }

    // Set busy Icon and home page

    switch (s) {
    case StateManager::Opening:
    case StateManager::Closing:
        ui_stateWidget->setSVGIcon(":/icons/wait");
        ui_stateWidget->rotate();
        setPage(Home);
        break;

    case StateManager::LoadingDataBase:
    case StateManager::WritingDataBase:
    case StateManager::Connecting:
    case StateManager::Syncing:
        ui_stateWidget->setSVGIcon(":/icons/wait");
        ui_stateWidget->rotate();
        break;

    case StateManager::Idle:
    case StateManager::Unknown:
        ui_stateWidget->setSVGIcon("");
        ui_stateWidget->stopAnimation();
        break;
    }
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
    if (StateManager::i()->state() == StateManager::Closing)
    {
        QMessageBox::StandardButton r = QMessageBox::question(this,
                              tr("Closing Ramses..."),
                              tr("I'm already closing, do you want me to force quit?\n\nThis may cause some data loss if the sync is not finished yet.")
                              );
        if (r == QMessageBox::Yes) {
            StateManager::i()->forceQuit();
            return;
        }
    }

    if (m_closing) {
        event->ignore();
        return;
    }

    setPage(Home);
    StateManager::i()->quit();
    m_closing = true;
    event->ignore();
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
    ui_mainStack = DuUI::addStackedLayout(ui_centralWidget);

    // Landing page
    ui_landingPage = new LandingPage(this);
    ui_mainStack->addWidget(ui_landingPage);

    // Home page
    auto homePage = new HomePage(this);
    ui_mainStack->addWidget(homePage);

    // Admin
    ui_adminPage = new SettingsWidget(this);
    ui_mainStack->addWidget(ui_adminPage);

    // Admin tabs
    UserManagerWidget *userManager = new UserManagerWidget(this);
    ui_adminPage->addPage(userManager,"Users", DuIcon(":/icons/users"));

    ProjectManagerWidget *projectManager = new ProjectManagerWidget(this);
    ui_adminPage->addPage(projectManager, "Projects", DuIcon(":/icons/projects"));

    TemplateStepManagerWidget *templateStepManager = new TemplateStepManagerWidget(this);
    ui_adminPage->addPage(templateStepManager, "Template Steps", DuIcon(":/icons/steps"));

    TemplateAssetGroupManagerWidget *templateAssetGroupManager = new TemplateAssetGroupManagerWidget(this);
    ui_adminPage->addPage(templateAssetGroupManager, "Template Asset Groups", DuIcon(":/icons/asset-groups"));

    StateManagerWidget *stateManager = new StateManagerWidget(this);
    ui_adminPage->addPage(stateManager, "States", DuIcon(":/icons/state"));

    FileTypeManagerWidget *fileTypeManager = new FileTypeManagerWidget(this);
    ui_adminPage->addPage(fileTypeManager, "File Types", DuIcon(":/icons/files"));

    ApplicationManagerWidget *applicationManager = new ApplicationManagerWidget(this);
    ui_adminPage->addPage(applicationManager, "Applications", DuIcon(":/icons/applications"));

    DBManagerWidget *dbManager = new DBManagerWidget(this);
    ui_adminPage->addPage(dbManager, "Database tools", DuIcon(":/icons/applications"));

// Pipeline editor
#ifndef DEACTIVATE_PIPELINE
    PipelineWidget *pipelineEditor = new PipelineWidget(this);
    ui_mainStack->addWidget(pipelineEditor);
    connect(pipelineEditor, &PipelineWidget::closeRequested, this, [this] () { setPage(Home); });
    qDebug() << "> Pipeline ready";//*/
#endif

#ifndef DEACTIVATE_ASSETSTABLE
    ItemManagerWidget *assetsTable = new ItemManagerWidget(RamStep::AssetProduction, this);
    ui_mainStack->addWidget(assetsTable);
    connect(assetsTable, &ItemManagerWidget::closeRequested, this, [this] () { setPage(Home); });
    qDebug() << "> Assets table ready";
#endif

#ifndef DEACTIVATE_SHOTSTABLE
    ItemManagerWidget *shotsTable = new ItemManagerWidget(RamStep::ShotProduction, this);
    ui_mainStack->addWidget(shotsTable);
    connect(shotsTable, &ItemManagerWidget::closeRequested, this, [this] () { setPage(Home); });
    qDebug() << "> Shots table ready";
#endif

#ifndef DEACTIVATE_SCHEDULE
    ScheduleManagerWidget *scheduleTable = new ScheduleManagerWidget(this);
    ui_mainStack->addWidget(scheduleTable);
    connect(scheduleTable,&ScheduleManagerWidget::closeRequested, this, [this] () { setPage(Home); });
    qDebug() << "> Schedule ready";
#endif

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
    auto *consoleFrame = new ConsoleDock(this);

    ui_consoleDockWidget = new DuDockWidget("Console");
    ui_consoleDockWidget->setWindowIcon(DuIcon(":/icons/bash"));
    ui_consoleDockWidget->setObjectName("consoleDock");

    auto *consoleTitle = new DuDockTitleWidget("Console", this);
    consoleTitle->setObjectName("dockTitle");
    consoleTitle->setIcon(":/icons/bash");
    ui_consoleDockWidget->setTitleBarWidget(consoleTitle);

    ui_consoleDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_consoleDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_consoleDockWidget->setWidget( consoleFrame );
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
}

void MainWindow::setupToolBars()
{
    ui_leftToolBar->addAction(m_actionAdmin);

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

    ui_pipelineMenuAction = ui_leftToolBar->addWidget(ui_pipelineButton);
    ui_assetMenuAction = ui_leftToolBar->addWidget(ui_assetsButton);
    ui_shotMenuAction = ui_leftToolBar->addWidget(ui_shotsButton);
    ui_scheduleMenuAction = ui_leftToolBar->addWidget(ui_scheduleButton);
    ui_filesMenuAction = ui_leftToolBar->addWidget(ui_filesButton);

    m_actionAdmin->setVisible(false);
    ui_pipelineMenuAction->setVisible(false);
    ui_shotMenuAction->setVisible(false);
    ui_assetMenuAction->setVisible(false);
    ui_scheduleMenuAction->setVisible(false);
    ui_filesMenuAction->setVisible(false);

    ui_stateWidget = new DuIconWidget(this);
    ui_rightToolBar->addWidget(ui_stateWidget);

    ui_ioWidget = new DuIconWidget(this);
    ui_rightToolBar->addWidget(ui_ioWidget);
    //_ioTimer.setSingleShot(true);


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

    ui_userMenuAction = ui_rightToolBar->addWidget(ui_userButton);
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

    ui_databaseMenuAction = ui_rightToolBar->addWidget(ui_databaseButton);
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

    ui_rightToolBar->addWidget(moreButton);

    updateWindow();
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
