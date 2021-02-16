#include "mainwindow.h"

MainWindow::MainWindow(QStringList /*args*/, QWidget *parent) :
    QMainWindow(parent)
{
    // Build the form
    setupUi(this);

    // Populate Toolbar
    userMenu = new QMenu();
    userMenu->addAction(actionUserProfile);
    userMenu->addAction(actionLogOut);
    userButton = new QToolButton(this);
    userButton->setIcon(QIcon(":/icons/user-settings"));
    userButton->setText("");
    userButton->setMenu(userMenu);
    userButton->setPopupMode(QToolButton::MenuButtonPopup);
    mainToolBar->addWidget(userButton);

    userButton->hide();

    mainToolBar->addAction(actionLogIn);

    mainToolBar->addAction(actionAdmin);
    actionAdmin->setVisible(false);

    //Populate status bar
    networkButton = new QToolButton(this);
    networkButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    networkButton->setText("Offline");
    networkButton->setMinimumWidth(75);
    mainStatusBar->addPermanentWidget(networkButton);

    userRoleButton = new QToolButton();
    userRoleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    userRoleButton->setText("Guest");
    userRoleButton->setMinimumWidth(75);
    mainStatusBar->addPermanentWidget(userRoleButton);

    // Add default stuff
    duqf_initUi();

    // Add settings
    ServerSettingsWidget *csw = new ServerSettingsWidget(this);
    settingsWidget->addPage(csw, "Server", QIcon(":/icons/server-settings"));

    // Add pages
    LoginPage *lp = new LoginPage(this);
    mainLayout->addWidget(lp);
    UserProfilePage *up = new UserProfilePage(this);
    mainStack->addWidget(up);
    adminPage = new SettingsWidget(this);
    mainStack->addWidget(adminPage);
    // Admin tabs
    adminPage->addPage(new UsersManagerWidget(this),"Users", QIcon(":/icons/users"));
    adminPage->addPage(new ProjectsManagerWidget(this), "Projects", QIcon(":/icons/projects"));

    // Set UI
    mainStack->setCurrentIndex(0);

    // Connect events
    connect(actionLogIn,SIGNAL(triggered()), this, SLOT(loginAction()));
    connect(actionLogOut,SIGNAL(triggered()), this, SLOT(logoutAction()));
    connect(actionUserProfile,SIGNAL(triggered()), this, SLOT(userProfile()));
    connect(actionAdmin,SIGNAL(triggered(bool)), this, SLOT(admin(bool)));
    connect(userButton,SIGNAL(clicked()),this, SLOT(userButton_clicked()));
    connect(networkButton,SIGNAL(clicked()),this, SLOT(networkButton_clicked()));
    connect(userRoleButton,SIGNAL(clicked()),this, SLOT(userProfile()));
    connect(mainStack,SIGNAL(currentChanged(int)), this, SLOT(pageChanged(int)));
    connect(lp, &LoginPage::serverSettings, this, &MainWindow::serverSettings);
    connect(DBInterface::instance(),&DBInterface::log, this, &MainWindow::log);
    connect(Ramses::instance(),&Ramses::loggedIn, this, &MainWindow::loggedIn);
    connect(Ramses::instance(),&Ramses::loggedOut, this, &MainWindow::loggedOut);
    connect(DBInterface::instance(),&DBInterface::connectionStatusChanged, this, &MainWindow::dbiConnectionStatusChanged);

    // Set style
    duqf_setStyle();
}

void MainWindow::duqf_initUi()
{
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
    duqf_closeSettingsButton->setObjectName("windowButton");

    AppearanceSettingsWidget *asw = new AppearanceSettingsWidget();
    settingsWidget->addPage(asw, "Appearance", QIcon(":/icons/color"));

    // ====== CONNECTIONS ======
    connect(duqf_maximizeButton,SIGNAL(clicked()),this,SLOT(duqf_maximize()));
#ifndef Q_OS_MAC
    connect(minimizeButton,SIGNAL(clicked()),this,SLOT(showMinimized()));
#endif
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(duqf_settingsButton, SIGNAL(clicked(bool)), this, SLOT(duqf_settings(bool)));
    connect(duqf_closeSettingsButton, SIGNAL(clicked()), this, SLOT(duqf_closeSettings()));
    connect(duqf_reinitSettingsButton, SIGNAL(clicked()), this, SLOT(duqf_reinitSettings()));
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

void MainWindow::log(QString m, LogUtils::LogType type)
{
    if (m == "") return;

    if (type == LogUtils::Debug)
    {
        qDebug().noquote() << m;
    }
    else if (type == LogUtils::Information)
    {
        mainStatusBar->showMessage(m,5000);
        qInfo().noquote() << m;
    }
    else if (type == LogUtils::Warning)
    {
        mainStatusBar->showMessage(m,10000);
        qWarning().noquote() << m;
    }
    else if (type == LogUtils::Critical)
    {
        mainStatusBar->showMessage(m);
        qCritical().noquote() << m;
    }
    else if (type == LogUtils::Fatal)
    {
        mainStatusBar->showMessage(m);
    }
    else if (type == LogUtils::Remote)
    {
        qDebug().noquote() << "REMOTE === " + m;
    }
}

void MainWindow::pageChanged(int i)
{

    actionAdmin->setChecked(i == 3);
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

void MainWindow::userProfile()
{
    mainStack->setCurrentIndex(2);
}

void MainWindow::admin(bool show)
{
    if (show) mainStack->setCurrentIndex(3);
    else mainStack->setCurrentIndex(0);
}

void MainWindow::userButton_clicked()
{
    if (Ramses::instance()->isConnected()) userProfile();
    else loginAction();
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
    userButton->setVisible(true);

    RamUser *user = Ramses::instance()->currentUser();
    disconnect(_currentUserConnection);
    _currentUserConnection = connect(user, &RamUser::changed, this, &MainWindow::currentUserChanged);
    currentUserChanged();
}

void MainWindow::loggedOut()
{
    disconnect(_currentUserConnection);

    actionLogIn->setVisible(true);
    userButton->setVisible(false);

    actionAdmin->setVisible(false);
    userRoleButton->setText("Guest");
    userRoleButton->setIcon(QIcon(""));
}

void MainWindow::currentUserChanged()
{
    RamUser *user = Ramses::instance()->currentUser();

    userRoleButton->setText(user->shortName());

    if (user->role() == RamUser::Admin)
    {
        actionAdmin->setVisible(true);
        userRoleButton->setIcon(QIcon(":/icons/admin"));

    }
    else if (user->role() == RamUser::Lead)
    {
        actionAdmin->setVisible(false);
        actionAdmin->setChecked(false);
        userRoleButton->setIcon(QIcon(":/icons/lead"));
    }
    else
    {
        actionAdmin->setVisible(false);
        actionAdmin->setChecked(false);
        userRoleButton->setIcon(QIcon(":/icons/user"));
    }
}

void MainWindow::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s == NetworkUtils::Online) networkButton->setText("Online");
    else if (s == NetworkUtils::Connecting) networkButton->setText("Connecting...");
    else if (s == NetworkUtils::Offline) networkButton->setText("Offline");
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
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
  else if (event->type() == QEvent::MouseMove)
  {
      if (this->isMaximized()) return false;
    QMouseEvent *mouseEvent = (QMouseEvent*)event;
    if (mouseEvent->buttons() & Qt::LeftButton && duqf_toolBarClicked)
    {
        this->move(mouseEvent->globalPos() - duqf_dragPosition);
        event->accept();
    }
    return true;
  }
  else if (event->type() == QEvent::MouseButtonRelease)
  {
      duqf_toolBarClicked = false;
      return true;
  }
#ifndef Q_OS_MAC
  else if (event->type() == QEvent::MouseButtonDblClick)
  {
      duqf_maximize();
      event->accept();
      return true;
  }
#endif
  else
  {
      // standard event processing
      return QObject::eventFilter(obj, event);
  }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    DBInterface::instance()->suspend(true);
    QFontDatabase::removeAllApplicationFonts();
    event->accept();
}
