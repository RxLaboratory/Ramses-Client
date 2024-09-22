#include "mainwindow.h"

#include <QFontDatabase>

#include "assetgroupmanagerwidget.h"
#include "assetmanagerwidget.h"
#include "docks/consoledockwidget.h"
#include "duapp/app-config.h"
#include "duwidgets/duqfupdatedialog.h"
#include "homepagewidget.h"
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
#include "docks/settingsdockwidget.h"
#include "ramses.h"
#include "duapp/duui.h"
#include "config.h"

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
    setPage(LandingPage);

    // Show new updates if any
#ifndef QT_DEBUG
    updateCheck();
#else
    updateCheck(true);
#endif

    // Everything is ready
    connectEvents();

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
}

void MainWindow::connectEvents()
{
    // Connect events

    // ==== Toolbar buttons ====

    connect(m_actionCloseProject, &QAction::triggered, this, []() { StateManager::i()->restart(); });
    connect(m_actionSetOnline, &QAction::triggered, this, &MainWindow::setOnlineAction);
    connect(m_actionUserFolder,SIGNAL(triggered()), this, SLOT(revealUserFolder()));
    connect(m_actionSync, SIGNAL(triggered()), DBInterface::i(),SLOT(sync()));
    connect(m_actionFullSync, SIGNAL(triggered()), DBInterface::i(),SLOT(fullSync()));

    connect(m_actionUpdate, &QAction::triggered,
            this, [this] () {
        if (!updateCheck(true))
            QMessageBox::information(
                this,
                tr("Already up-to-date"),
                tr("This application is already up-to-date, congrats!"));
    });

    connect(m_actionUserProfile,&QAction::triggered,
            this, [] () {
        RamUser *user = Ramses::i()->currentUser();
        if (user)
            user->edit(true);
    });

    // ==== Pages ====

    connect(m_actionAdmin,&DuAction::triggered,
            this, [this] () { setPage(AdminPage); } );
    connect(m_actionPipeline,&DuAction::triggered,
            this, [this] () { setPage(PipeLinePage); } );
    connect(m_actionShots,&DuAction::triggered,
            this, [this] () { setPage(ShotsPage); } );
    connect(m_actionAssets,&DuAction::triggered,
            this, [this] () { setPage(AssetsPage); } );
    connect(m_actionSchedule,&DuAction::triggered,
            this, [this] () { setPage(SchedulePage); } );

    // ==== Docks ====

    connect(m_actionConsole, &DuAction::triggered, this, [this] () { setDock(ConsoleDock); });
    connect(m_actionSettings, &DuAction::triggered, this, [this] () { setDock(SettingsDock); });
    connect(m_stepsAction, &DuAction::triggered, this, [this] () { setDock(StepsDock); });
    connect(m_pipeFilesAction, &DuAction::triggered, this, [this] () { setDock(PipeFilesDock); });
    connect(m_assetGroupAction, &DuAction::triggered, this, [this] () { setDock(AssetGroupsDock); });
    connect(m_assetListAction, &DuAction::triggered, this, [this] () { setDock(AssetsDock); });
    connect(m_sequenceAction, &DuAction::triggered, this, [this] () { setDock(SequencesDock); });
    connect(m_shotListAction, &DuAction::triggered, this, [this] () { setDock(ShotsDock); });
    connect(m_actionStatistics, &DuAction::triggered, this, [this] () { setDock(StatisticsDock); });
    connect(m_actionTimeline, &DuAction::triggered, this, [this] () { setDock(TimelineDock); });

    connect(m_projectAction, &DuAction::triggered, this, [] () {
        RamProject *proj = Ramses::i()->project();
        if (proj)
            proj->edit();
    });

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

    // ==== State ====

    connect(StateManager::i(), &StateManager::stateChanged, this, &MainWindow::changeState);
    connect(LocalDataInterface::i(), &LocalDataInterface::stateChanged, this, &MainWindow::changeIOState);

    connect(Daemon::instance(), &Daemon::raise, this, &MainWindow::raise);

    connect(Ramses::i(), &Ramses::ready, this, &MainWindow::ramsesReady);
    connect(Ramses::i(), &Ramses::roleChanged, this, &MainWindow::changeUserRole);

    connect(qApp, &QCoreApplication::aboutToQuit, this, &MainWindow::onQuit);
}

void MainWindow::setPropertiesDockWidget(QWidget *w, QString title, QString icon, bool temporary)
{

    QWidget *previous = ui_propertiesDockWidget->widget();
    if (previous) {
        disconnect(previous, nullptr, ui_propertiesDockWidget, nullptr);
    }
    connect(w, &QWidget::destroyed, ui_propertiesDockWidget, &QDockWidget::hide);

    ui_propertiesDockWidget->setWidget( w );
    ui_propertiesDockWidget->setAutoDeleteWidget(temporary);
    ui_propertiesDockWidget->dockTitleWidget()->setTitle(title);
    ui_propertiesDockWidget->dockTitleWidget()->setIcon(icon);
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

void MainWindow::setOnlineAction()
{
    /*DBInterface::i()->setOnline();

    // Trigger a full sync
    if (RamServerInterface::instance()->isOnline()) DBInterface::i()->fullSync();*/
}

void MainWindow::setPage(Page p)
{
    if (m_currentPage == p)
        return;

    if (p == LandingPage ||
        p == HomePage) {
        ui_mainStack->setCurrentIndex(p);
        m_currentPage = p;
        return;
    }

    // Without project,
    // get home!
    if (!Ramses::i()->project()) {
        setPage(HomePage);
        return;
    }

    // Delete previous if any
    auto i = ui_mainStack->takeAt(2);
    if (i) {
        auto w = i->widget();
        if (w)
            w->deleteLater();
        delete i;
    }

    // Build the page and show it
    QWidget *newWidget = nullptr;

    switch(p) {
    case LandingPage:
    case HomePage:
        return;
    case AdminPage:
        // TODO
        break;
    case PipeLinePage:
        newWidget = new PipelineWidget(this);
        break;
    case AssetsPage:
        newWidget = new ItemManagerWidget(RamStep::AssetProduction, this);
        break;
    case ShotsPage:
        newWidget = new ItemManagerWidget(RamStep::ShotProduction, this);
        break;
    case SchedulePage:
        newWidget = new ScheduleManagerWidget(this);
        break;
    }

    if (!newWidget) {
        setPage(HomePage);
        return;
    }

    ui_mainStack->insertWidget(2, newWidget);
    ui_mainStack->setCurrentIndex(2);
    m_currentPage = p;
}

void MainWindow::setDock(Dock d)
{
    DuDockWidget *dock = nullptr;

    switch(d) {
    case SettingsDock:
        dock = ui_settingsDock;
        break;
    case ConsoleDock:
        dock = ui_consoleDockWidget;
        break;
    case PropertiesDock:
        dock = ui_propertiesDockWidget;
        break;
    case StatisticsDock:
        if (!ui_statsDockWidget->widget())
            ui_statsDockWidget->setWidget(new StatisticsWidget(this));
        dock = ui_statsDockWidget;
        break;
    case StepsDock:
        if (!ui_stepsDockWidget->widget())
            ui_stepsDockWidget->setWidget(new StepManagerWidget(this));
        dock = ui_stepsDockWidget;
        break;
    case PipeFilesDock:
        if (!ui_pipeFileDockWidget->widget())
            ui_pipeFileDockWidget->setWidget(new PipeFileManagerWidget(this));
        dock = ui_pipeFileDockWidget;
        break;
    case AssetGroupsDock:
        if (!ui_assetGroupsDockWidget->widget())
            ui_assetGroupsDockWidget->setWidget(new AssetGroupManagerWidget(this));
        dock = ui_assetGroupsDockWidget;
        break;
    case AssetsDock:
        if (!ui_assetsDockWidget->widget())
            ui_assetsDockWidget->setWidget(new AssetManagerWidget(this));
        dock = ui_assetsDockWidget;
        break;
    case SequencesDock:
        if (!ui_sequencesDockWidget->widget())
            ui_sequencesDockWidget->setWidget(new SequenceManagerWidget(this));
        dock = ui_sequencesDockWidget;
        break;
    case ShotsDock:
        if (!ui_shotsDockWidget->widget())
            ui_shotsDockWidget->setWidget(new ShotManagerWidget(this));
        dock = ui_shotsDockWidget;
        break;
    case TimelineDock:
        if (!ui_timelineDockWidget->widget())
            ui_timelineDockWidget->setWidget(new TimelineWidget(this));
        dock = ui_timelineDockWidget;
        break;
    }

    if (!dock)
        return;

    dock->show();
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
        //m_actionAdmin->setVisible(true);
        //break;
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

    ui_userMenuAction->setText(QString("User (%1)").arg(user->shortName()));
    ui_userMenuAction->setVisible(true);
    m_actionUserProfile->setVisible(true);
    m_actionUserFolder->setVisible(true);
    m_actionCloseProject->setVisible(true);

    // Set Project

    // Guaranted to exist,
    // The signal is emited only if all are ready
    RamProject *project = Ramses::i()->project();

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

void MainWindow::changeState(StateManager::State s)
{
    // Set busy Icon and home page
    switch (s) {
    case StateManager::Opening:
    case StateManager::Closing:
        ui_stateWidget->setColor(focusStateIconColor());
        ui_stateWidget->setSVGIcon(":/icons/wait");
        ui_stateWidget->rotate();
        hideAllDocks();
        setPage(HomePage);
        ui_leftToolBar->setEnabled(false);
        ui_rightToolBar->setEnabled(false);
        break;

    case StateManager::Connecting:
    case StateManager::Syncing:
        ui_rightToolBar->setEnabled(false);
        ui_stateWidget->setColor(focusStateIconColor());
        ui_stateWidget->setSVGIcon(":/icons/wait");
        ui_stateWidget->rotate();
        break;

    case StateManager::Idle:
    case StateManager::Unknown:
        ui_leftToolBar->setEnabled(true);
        ui_rightToolBar->setEnabled(true);
        ui_stateWidget->stopAnimation();
        changeServerState(RamServerClient::i()->status());
        break;
    }

    ui_stateWidget->setToolTip(StateManager::i()->title());
}

void MainWindow::changeIOState(LocalDataInterface::State s)
{
    ui_stateWidget->setToolTip("");

    // Set the read/write icon
    switch(s) {
    case LocalDataInterface::Idle:
        if (DBInterface::i()->isTeamProject()) {
            if (RamServerClient::i()->ssl()) {
                ui_ioWidget->setColor(normalStateIconColor());
                ui_ioWidget->setSVGIcon(":/icons/secure");
                ui_stateWidget->setToolTip(tr("Secured connection"));
            }
            else {
                ui_ioWidget->setColor(warningStateIconColor());
                ui_ioWidget->setSVGIcon(":/icons/insecure");
                ui_stateWidget->setToolTip(tr("Warning: insecure connection!"));
            }
        }
        else {
            ui_ioWidget->setColor(normalStateIconColor());
            ui_ioWidget->setSVGIcon("");
        }
        break;
    case LocalDataInterface::Reading:
        ui_ioWidget->setColor(normalStateIconColor());
        ui_ioWidget->setSVGIcon(":/icons/database-read");
        break;
    case LocalDataInterface::Writing:
        ui_ioWidget->setColor(normalStateIconColor());
        ui_ioWidget->setSVGIcon(":/icons/database-write");
        break;
    case LocalDataInterface::ReadWrite:
        ui_ioWidget->setColor(normalStateIconColor());
        ui_ioWidget->setSVGIcon(":/icons/database-read-write");
        break;
    }
}

void MainWindow::changeServerState(RamServerClient::ClientStatus s)
{
    if (StateManager::i()->state() != StateManager::Idle &&
        StateManager::i()->state() != StateManager::Unknown)
        return;

    if (!DBInterface::i()->isTeamProject()) {
        ui_stateWidget->setColor(normalStateIconColor());
        ui_stateWidget->setSVGIcon(":/icons/local");
        ui_stateWidget->setToolTip(tr("Local project"));
        ui_serverMenuAction->setVisible(false);
        updateWindow();
        return;
    }

    ui_serverMenuAction->setVisible(true);
    updateWindow();

    if (s == RamServerClient::Offline) {
        ui_stateWidget->setColor(warningStateIconColor());
        ui_stateWidget->setSVGIcon(":/icons/offline");
        ui_stateWidget->setToolTip(tr("Team project\nWarning: Offline!"));
        m_actionSync->setVisible(false);
        m_actionFullSync->setVisible(false);
        m_actionSetOnline->setVisible(true);
        return;
    }

    ui_stateWidget->setColor(normalStateIconColor());
    ui_stateWidget->setSVGIcon(":/icons/online");
    ui_stateWidget->setToolTip(tr("Team project"));
    m_actionSetOnline->setVisible(false);
    m_actionSync->setVisible(true);
    m_actionFullSync->setVisible(true);

    switch(s) {
    case RamServerClient::Offline:
        break;
    case RamServerClient::Online:
    case RamServerClient::Ready:
        m_actionSync->setEnabled(true);
        m_actionFullSync->setEnabled(true);
        break;
    case RamServerClient::Syncing:
        m_actionSync->setEnabled(false);
        m_actionFullSync->setEnabled(false);
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

QColor MainWindow::normalStateIconColor()
{
    return DuUI::pushColor(
        DuSettings::i()->get(DuSettings::UI_IconColor).value<QColor>(), 2
        );
}

QColor MainWindow::warningStateIconColor()
{
    return DuSettings::i()->get(DuSettings::UI_IconColor).value<QColor>();
}

QColor MainWindow::focusStateIconColor()
{
    return DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>();
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

    setPage(HomePage);
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

    m_actionCloseProject = new DuAction(this);
    m_actionCloseProject->setText(tr("Close project"));
    m_actionCloseProject->setToolTip(tr("Logs you out of the Ramses database."));
    m_actionCloseProject->setIcon(":/icons/logout");

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
    m_actionStatistics->setToolTip(tr("Shows some project statics and production tracking data."));
    m_actionStatistics->setIcon(":/icons/stats");

    m_actionTimeline = new DuAction(this);
    m_actionTimeline->setText(tr("Timeline"));
    m_actionTimeline->setToolTip(tr("Shows the shots in a timeline."));
    m_actionTimeline->setIcon(":/icons/timeline");

    m_actionSetOnline = new DuAction(this);
    m_actionSetOnline->setText(tr("Set online (sign in)"));
    m_actionSetOnline->setToolTip(tr("Connects to the Ramses server."));
    m_actionSetOnline->setIcon(":/icons/online");

    m_actionSettings = new DuAction(this);
    m_actionSettings->setText(tr("Settings"));
    m_actionSettings->setToolTip(tr("Edit the application settings."));
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

    m_actionUpdate = new DuAction(this);
    m_actionUpdate->setText(tr("Ramses update..."));
    m_actionUpdate->setToolTip(tr("Checks if an update of the application is available."));
    m_actionUpdate->setIcon(":/icons/check-update");

    m_projectAction = new DuAction(tr("Project settings"), this);
    m_projectAction->setIcon(":/icons/project");
    m_projectAction->setToolTip(tr("Edit the project settings."));

    m_stepsAction = new DuAction(tr("Steps"), this);
    m_stepsAction->setIcon(":/icons/step");
    m_stepsAction->setToolTip(tr("Shows the list of steps."));

    m_pipeFilesAction = new DuAction(tr("Pipe formats"), this);
    m_pipeFilesAction->setIcon(":/icons/connection");
    m_pipeFilesAction->setToolTip(tr("Shows the list of pipes."));

    m_assetGroupAction = new DuAction(tr("Asset groups"), this);
    m_assetGroupAction->setIcon(":/icons/asset-group");
    m_assetGroupAction->setToolTip(tr("Shows the list of asset groups."));

    m_assetListAction = new DuAction(tr("Assets"), this);
    m_assetListAction->setIcon(":/icons/asset");
    m_assetListAction->setToolTip(tr("Shows the list of assets."));
    m_assetListAction->setCheckable(true);

    m_sequenceAction = new DuAction(tr("Sequences"), this);
    m_sequenceAction->setIcon(":/icons/sequence");
    m_sequenceAction->setToolTip(tr("Shows the list of sequences."));

    m_shotListAction = new DuAction(tr("Shots"), this);
    m_shotListAction->setIcon(":/icons/shot");
    m_shotListAction->setToolTip(tr("Shows the list of shots."));

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
    ui_landingPage = new LandingPageWidget(this);
    ui_mainStack->addWidget(ui_landingPage);

    // Home page
    auto homePage = new HomePageWidget(this);
    ui_mainStack->addWidget(homePage);

    // Admin
    ui_adminPage = new SettingsWidget(this);
    ui_mainStack->addWidget(ui_adminPage);

    // Admin tabs
    /*UserManagerWidget *userManager = new UserManagerWidget(this);
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
    ui_adminPage->addPage(dbManager, "Database tools", DuIcon(":/icons/applications"));*/
}

void MainWindow::setupDocks()
{
    QSettings settings;

    // Settings Dock
    ui_settingsDock = new DuDockWidget("Settings", ":/icons/settings", this);
    ui_settingsDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_settingsDock->setFeatures(DuDockWidget::DockWidgetClosable | DuDockWidget::DockWidgetMovable | DuDockWidget::DockWidgetFloatable );
    ui_settingsDock->setWidget(new SettingsDockWidget(ui_settingsDock));
    this->addDockWidget(Qt::RightDockWidgetArea, ui_settingsDock);
    ui_settingsDock->hide();

    // A console in a tab
    ui_consoleDockWidget = new DuDockWidget("Console", ":/icons/bash", this);
    ui_consoleDockWidget->setAllowedAreas(Qt::AllDockWidgetAreas);
    ui_consoleDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    ui_consoleDockWidget->setWidget( new ConsoleDockWidget(this) );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_consoleDockWidget);
    ui_consoleDockWidget->hide();

    // The properties dock
    ui_propertiesDockWidget = new DuDockWidget("Properties", ":/icons/asset", this);
    ui_propertiesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_propertiesDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::RightDockWidgetArea, ui_propertiesDockWidget);
    ui_propertiesDockWidget->hide();

    // The timeline
    ui_timelineDockWidget = new DuDockWidget(tr("Timeline"), ":/icons/timeline", this);
    ui_timelineDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
    ui_timelineDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::BottomDockWidgetArea, ui_timelineDockWidget);
    ui_timelineDockWidget->setAutoDeleteWidget(true);
    ui_timelineDockWidget->hide();


    ui_statsDockWidget = new DuDockWidget(tr("Statistics"), ":/icons/stats", this);
    ui_statsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_statsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    Qt::DockWidgetArea area = static_cast<Qt::DockWidgetArea>( settings.value("ui/statsArea", Qt::LeftDockWidgetArea).toInt() );
    this->addDockWidget(area, ui_statsDockWidget);
    ui_statsDockWidget->setAutoDeleteWidget(true);
    ui_statsDockWidget->hide();

    // The steps dock
    ui_stepsDockWidget = new DuDockWidget(tr("Steps"), ":/icons/step", this);
    ui_stepsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_stepsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_stepsDockWidget);
    ui_stepsDockWidget->setAutoDeleteWidget(true);
    ui_stepsDockWidget->hide();

    // The pipe formats dock
    ui_pipeFileDockWidget = new DuDockWidget(tr("Pipe formats"), ":/icons/connection", this);
    ui_pipeFileDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_pipeFileDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_pipeFileDockWidget);
    ui_pipeFileDockWidget->setAutoDeleteWidget(true);
    ui_pipeFileDockWidget->hide();

    // The asset groups dock
    ui_assetGroupsDockWidget = new DuDockWidget(tr("Asset Groups"), ":/icons/asset-group", this);
    ui_assetGroupsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_assetGroupsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_assetGroupsDockWidget);
    ui_assetGroupsDockWidget->setAutoDeleteWidget(true);
    ui_assetGroupsDockWidget->hide();

    // The assets dock
    ui_assetsDockWidget = new DuDockWidget(tr("Assets"), ":/icons/asset", this);
    ui_assetsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_assetsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_assetsDockWidget);
    ui_assetsDockWidget->setAutoDeleteWidget(true);
    ui_assetsDockWidget->hide();

    // The sequences dock
    ui_sequencesDockWidget = new DuDockWidget(tr("Sequences"), ":/icons/sequence", this);
    ui_sequencesDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_sequencesDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_sequencesDockWidget);
    ui_sequencesDockWidget->setAutoDeleteWidget(true);
    ui_sequencesDockWidget->hide();

    // The Shots dock
    ui_shotsDockWidget = new DuDockWidget(tr("Shots"), ":/icons/shot", this);
    ui_shotsDockWidget->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    ui_shotsDockWidget->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable );
    this->addDockWidget(Qt::LeftDockWidgetArea, ui_shotsDockWidget);
    ui_shotsDockWidget->setAutoDeleteWidget(true);
    ui_shotsDockWidget->hide();
}

bool MainWindow::updateCheck(bool force)
{
    // Check for updates, right during startup
    qInfo() << "Update check...";

    QDateTime lastCheck = DuSettings::i()->get(DuSettings::APP_LastUpdateCheck).toDateTime();
    qDebug().noquote() << "Last check was on: " + lastCheck.toString(DATETIME_DATA_FORMAT);
    int days = lastCheck.daysTo(QDateTime::currentDateTime());
    qDebug().noquote() << days << " days since last check.";

    if (
        days > 0 ||
        !lastCheck.isValid() ||
        lastCheck.isNull() ||
        force
        )
    {
        DuApplication::i()->checkUpdate(true);
        DuSettings::i()->set(DuSettings::APP_LastUpdateCheck, QDateTime::currentDateTime());
        QJsonObject updateInfo = DuApplication::i()->updateInfo();

        if (updateInfo.value("update").toBool())
        {
            DuQFUpdateDialog dialog(updateInfo);
            if (DuUI::execDialog(&dialog))
                StateManager::i()->quit(true, 500); // We need a delay in case the app is still starting
            return true;
        }
    }
    else
    {
        qDebug() << "We'll check again tomorrow.";
    }

    return false;
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
    ui_stateWidget->setColor(
        DuSettings::i()->get(DuSettings::UI_FocusColor).value<QColor>()
        );
    ui_rightToolBar->addWidget(ui_stateWidget);

    ui_ioWidget = new DuIconWidget(this);
    ui_ioWidget->setColor( normalStateIconColor() );
    ui_rightToolBar->addWidget(ui_ioWidget);

    DuMenu *moreMenu = new DuMenu(this);

    ui_userMenu = new DuMenu(tr("User (%1)").arg(tr("Guest")));
    ui_userMenu->setIcon(DuIcon(":/icons/user"));

    ui_userMenu->addAction(m_actionUserFolder);
    m_actionUserFolder->setVisible(false);
    ui_userMenu->addAction(m_actionUserProfile);
    m_actionUserProfile->setVisible(false);

    ui_userMenuAction = moreMenu->addMenu(ui_userMenu);
    ui_userMenuAction->setVisible(false);

    ui_serverMenu = new DuMenu(tr("Server"));
    ui_serverMenu->setIcon(DuIcon(":/icons/server"));
    ui_serverMenu->addAction(m_actionSync);
    ui_serverMenu->addAction(m_actionFullSync);
    ui_serverMenu->addSeparator();
    ui_serverMenu->addAction(m_actionSetOnline);

    ui_serverMenuAction = moreMenu->addMenu(ui_serverMenu);
    ui_serverMenuAction->setVisible(false);

    moreMenu->addSeparator();
    moreMenu->addAction(m_actionConsole);
    moreMenu->addAction(m_actionSettings);
    moreMenu->addAction(m_actionUpdate);

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

    moreMenu->addSeparator();
    moreMenu->addAction(m_actionCloseProject);
    m_actionCloseProject->setVisible(false);

    QToolButton *moreButton = new QToolButton();
    moreButton->setText(tr("More"));
    moreButton->setIcon(DuIcon(":/icons/more"));
    moreButton->setPopupMode(QToolButton::InstantPopup);
    moreButton->setIconSize(QSize(16,16));
    moreButton->setMenu(moreMenu);
    moreButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

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
