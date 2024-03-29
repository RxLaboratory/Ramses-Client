#include "databasecreatewidget.h"

#include "duqf-utils/guiutils.h"
#include "duqf-utils/utils.h"
#include "ramuser.h"
#include "ramses.h"
#include "mainwindow.h"
#include "dbinterface.h"

DatabaseCreateWidget::DatabaseCreateWidget(QWidget *parent) :
    DuScrollArea(parent)
{
    setupUi();
    connectEvents();
}

void DatabaseCreateWidget::checkPath(QString p)
{
    if (!p.endsWith(".ramses", Qt::CaseInsensitive) && !p.endsWith(".sqlite", Qt::CaseInsensitive))
    {
        p += ".ramses";
        ui_fileSelector->setPath(p);
    }
}

void DatabaseCreateWidget::createDB()
{
    // Offline
    if (ui_tabWidget->currentIndex() == 0) createOfflineDB();
    // Online
    else createOnlineDB();
}

void DatabaseCreateWidget::finishSync()
{
    if (!m_downloading) return;
    ProgressManager *pm = ProgressManager::instance();
    pm->finish();
}

void DatabaseCreateWidget::setupUi()
{
    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(dummy);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(6,6,6,6);

    auto topWidget = new QWidget(dummy);
    topWidget->setProperty("class", "duBlock");
    mainLayout->addWidget(topWidget);

    QGridLayout *topLayout = new QGridLayout(topWidget);
    topLayout->setSpacing(3);
    topLayout->setContentsMargins(3,3,3,3);

    topLayout->addWidget(new QLabel(tr("Database file")), 0, 0);

    ui_fileSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, dummy);
    ui_fileSelector->setPlaceHolderText(tr("File path of the Ramses Database..."));
    ui_fileSelector->setDialogTitle(tr("Select the location of the Ramses Database."));
    ui_fileSelector->setMode(DuQFFolderSelectorWidget::Save);
    ui_fileSelector->setFilter(tr("Ramses (*.ramses);;SQLite (*.sqlite);;All Files (*.*)"));
    topLayout->addWidget(ui_fileSelector, 0, 1);

    topLayout->addWidget(new QLabel(tr("Ramses path")), 1, 0);

    ui_folderSelector = new DuQFFolderSelectorWidget();
    ui_folderSelector->showRevealButton(false);
    ui_folderSelector->setPlaceHolderText(QDir::homePath() + "/Ramses");
    topLayout->addWidget(ui_folderSelector, 1, 1);

    ui_tabWidget = new QTabWidget(dummy);
    ui_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainLayout->addWidget(ui_tabWidget);

    QWidget *offlineWidget = new QWidget(ui_tabWidget);
    offlineWidget->setProperty("class", "duBlock");
    ui_tabWidget->addTab(offlineWidget, QIcon(":/icons/folder"), tr("Offline"));
    QGridLayout *offlineLayout = new QGridLayout(offlineWidget);
    offlineLayout->setAlignment(Qt::AlignTop);
    offlineLayout->setSpacing(3);
    offlineLayout->setContentsMargins(3,3,3,3);

    QLabel *userLabel = new QLabel("User ID", dummy);
    offlineLayout->addWidget(userLabel, 0, 0);

    ui_shortNameEdit = new QLineEdit(dummy);
    offlineLayout->addWidget(ui_shortNameEdit, 0, 1);

    QLabel *userNameLabel = new QLabel(tr("User name"), this);
    offlineLayout->addWidget(userNameLabel, 1, 0);

    ui_nameEdit = new QLineEdit(this);
    offlineLayout->addWidget(ui_nameEdit, 1, 1);

    QWidget *onlineWidget = new QWidget(ui_tabWidget);
    onlineWidget->setProperty("class", "duBlock");
    ui_tabWidget->addTab(onlineWidget, QIcon(":/icons/server-settings"), tr("Online (Sync)"));
    QGridLayout *onlineLayout = new QGridLayout(onlineWidget);
    onlineLayout->setAlignment(Qt::AlignTop);
    onlineLayout->setSpacing(3);
    onlineLayout->setContentsMargins(3,3,3,3);
    onlineLayout->setAlignment(Qt::AlignTop);

    ui_serverEdit = new ServerEditWidget();
    onlineLayout->addWidget(ui_serverEdit, 0, 0);

    ui_createButton = new QPushButton(tr("Create and log in"));
    ui_createButton->setIcon(QIcon(":/icons/apply"));
    mainLayout->addWidget(ui_createButton);

    mainLayout->addStretch();

#ifdef QT_DEBUG
    ui_fileSelector->setPath("/home/duduf/Documents/test.ramses");
    ui_shortNameEdit->setText("Duf");
    ui_nameEdit->setText("Nicolas Dufresne");

    ui_serverEdit->setAddress("localhost/ramses");
    ui_serverEdit->setSsl(false);
    ui_serverEdit->setPort(8001);
#endif
}

void DatabaseCreateWidget::connectEvents()
{
    connect(ui_fileSelector, &DuQFFolderSelectorWidget::pathChanged, this, &DatabaseCreateWidget::checkPath);
    connect(ui_createButton, &QPushButton::clicked, this, &DatabaseCreateWidget::createDB);
    connect(DBInterface::instance(), &DBInterface::syncFinished, this, &DatabaseCreateWidget::finishSync);
}

bool DatabaseCreateWidget::createNewDB()
{
    // Remove existing file
    QString newFilePath = ui_fileSelector->path();
    if (QFileInfo::exists(newFilePath))
    {
        QMessageBox::StandardButton ok = QMessageBox::question(this, tr("Confirm file overwrite"), tr("Are you sure you want to overwrite this file?") + "\n\n" + newFilePath);
        if (ok != QMessageBox::Yes) return false;
        FileUtils::remove(newFilePath);
    }

    // Copy the file
    FileUtils::copy(":/data/template", newFilePath);

    if (!QFileInfo::exists(newFilePath))
    {
        QMessageBox::warning(this, tr("I can't save the database"), tr("I'm sorry, I've failed to create the database at this location.\nMaybe you can try another location...") + "\n\n" + newFilePath );
        return false;
    }

    // Save Ramses Path
    LocalDataInterface::setRamsesPath(newFilePath, ui_folderSelector->path());

    // Set File
    DBInterface::instance()->setDataFile(newFilePath, true);

    return true;
}

bool DatabaseCreateWidget::createNewDB(ServerConfig s)
{
    // Remove existing file
    QString newFilePath = ui_fileSelector->path();
    if (QFileInfo::exists(newFilePath))
    {
        QMessageBox::StandardButton ok = QMessageBox::question(this, tr("Confirm file overwrite"), tr("Are you sure you want to overwrite this file?") + "\n\n" + newFilePath);
        if (ok != QMessageBox::Yes) return false;
        FileUtils::remove(newFilePath);
    }

    // Copy the file
    FileUtils::copy(":/data/template", newFilePath);

    if (!QFileInfo::exists(newFilePath))
    {
        QMessageBox::warning(this, tr("I can't save the database"), tr("I'm sorry, I've failed to create the database at this location.\nMaybe you can try another location...") + "\n\n" + newFilePath );
        return false;
    }

    // Add server settings
    LocalDataInterface::instance()->setServerSettings(ui_fileSelector->path(), s);

    // Save Ramses Path
    LocalDataInterface::setRamsesPath(newFilePath, ui_folderSelector->path());

    // Set File
    DBInterface::instance()->setDataFile(newFilePath, true);

    return true;
}

void DatabaseCreateWidget::createOnlineDB()
{
    // TODO

    // Get the server config
    ServerConfig s;
    s.address = ui_serverEdit->address();
    if (s.address == "") return;
    s.useSsl = ui_serverEdit->ssl();
    s.updateDelay = ui_serverEdit->updateFreq();
    s.timeout = ui_serverEdit->timeout();
    s.port = ui_serverEdit->port();

    // Create DB
    if (!createNewDB(s)) return;

    // Connect to server
    // The file and server settings are already set in createNewDB()
    // And the server is also already set online
    RamServerInterface *rsi = RamServerInterface::instance();

    // If we failed to get online
    if (!rsi->isOnline())
    {
        QMessageBox::information(this,
                                 tr("Server error"),
                                 tr("Sorry, I can't connect correctly to the server.\nPlease double check the server settings.")
                                 );
        return;
    }

    // If we're not logged in
    QString uuid = rsi->currentUserUuid();
    if (uuid == "")
    {
        QMessageBox::warning(this,
                                 tr("Login issue"),
                                 tr("I can't log you in for an unknown reason.\n\n"
                                    "Try again, or file a bug report.")
                                 );
        return;
    }

    // Hide dock
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->hidePropertiesDock();

    // And trigger the first Sync
    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("Downloading data..."));
    pm->freeze();
    pm->start();
    m_downloading = true;
    DBInterface::instance()->fullSync();
}

void DatabaseCreateWidget::createOfflineDB()
{
    // Check path
    if (ui_fileSelector->path() == "")
    {
        QMessageBox::warning(this, tr("Where should I save the database?"), tr("I'm sorry, you have to choose a file to create the database.") );
        return;
    }

    // Check name
    if (ui_shortNameEdit->text() == "")
    {
        QMessageBox::warning(this, tr("What's your name?"), tr("I'm sorry, you have to choose a user ID to create the database.") );
        return;
    }

    if (ui_nameEdit->text() == "")
    {
        QMessageBox::warning(this, tr("What's your name?"), tr("I'm sorry, you have to choose a user name to create the database.") );
        return;
    }

    if (!createNewDB()) return;

    // Create user
    RamUser *newUser = new RamUser(ui_shortNameEdit->text(), ui_nameEdit->text());
    newUser->setRole(RamUser::Admin);

    // Login
    Ramses::instance()->setUser( newUser );

    // Hide dock
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->hidePropertiesDock();
}
