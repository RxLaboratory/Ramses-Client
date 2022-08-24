#include "databasecreatewidget.h"

#include "dbinterface.h"
#include "duqf-utils/guiutils.h"
#include "duqf-utils/utils.h"
#include "ramuser.h"
#include "ramses.h"
#include "mainwindow.h"
#include "datacrypto.h"

DatabaseCreateWidget::DatabaseCreateWidget(QWidget *parent) :
    QScrollArea(parent)
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
    if (ui_tabWidget->currentIndex() == 0)
    {
        // Check path
        if (ui_fileSelector->path() == "")
        {
            QMessageBox::warning(this, tr("Where should I save the database?"), tr("I'm sorry, you have to choose a file to create the database.") );
            return;
        }

        // Check password
        if (ui_npassword1Edit->text() == "")
        {
            QMessageBox::warning(this, tr("What's your password?"), tr("I'm sorry, you have to choose a password to create the database.") );
            return;
        }
        if (ui_npassword1Edit->text() != ui_npassword2Edit->text())
        {
            QMessageBox::warning(this, tr("Password mismatch"), tr("I'm sorry, the two fields for the new password are different.\nPlease try again.") );
            ui_npassword1Edit->setText("");
            ui_npassword2Edit->setText("");
            return;
        }

        // Check name
        if (ui_shortNameEdit->text() == "")
        {
            QMessageBox::warning(this, tr("What's your name?"), tr("I'm sorry, you have to choose a user ID to create the database.") );
            return;
        }

        createNewDB();

        // Create user
        RamUser *newUser = new RamUser(ui_shortNameEdit->text(), ui_shortNameEdit->text());
        newUser->updatePassword("", ui_npassword1Edit->text());
        newUser->setRole(RamUser::Admin);
        Ramses::instance()->users()->append(newUser);

        // Login
        Ramses::instance()->setUser( newUser );

        // Hide dock
        MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
        mw->hidePropertiesDock();
    }
    // Online
    else
    {
        ServerConfig s;
        s.address = ui_serverEdit->address();
        if (s.address == "") return;
        s.useSsl = ui_serverEdit->ssl();
        s.updateDelay = ui_serverEdit->updateFreq();
        s.timeout = ui_serverEdit->timeout();

        // Connect to server
        RamServerInterface *rsi = RamServerInterface::instance();
        rsi->setServerAddress(s.address);
        rsi->setSsl(s.useSsl);
        rsi->setTimeout(s.timeout);

        // (try to ) set online
        rsi->setOnline();
        if (!rsi->isOnline())
        {
            QMessageBox::information(this,
                                     tr("Server error"),
                                     tr("Sorry, I can't connect correctly to the server.\nPlease double check the server settings.")
                                     );
            return;
        }

        // login
        QString password = ui_onlinePasswordEdit->text();
        password = DataCrypto::instance()->generatePassHash(password, s.address.replace("/", ""));

        QString uuid = rsi->login(ui_onlineShortNameEdit->text(), password);
        if (uuid == "")
        {
            QMessageBox::information(this,
                                     tr("Invalid user ID or password"),
                                     tr("Please check your ID and password.")
                                     );
            return;
        }

        // Download all data
        QJsonArray tables = rsi->downloadData();
        if (tables.count() == 0)
        {
            QMessageBox::warning(this,
                                     tr("Can't find any data"),
                                     tr("I can't download any data from this server.\n\n"
                                        "This may be due to a slow connexion,\n"
                                        "try to increase the server time out.")
                                     );
            return;
        }

        // Create DB
        createNewDB();

        // Save data to DB
        LocalDataInterface *ldi = LocalDataInterface::instance();
        ldi->sync(tables);

        // Wait for the data to be written
        ldi->waitForReady();

        if (!ldi->isReady())
        {
            QMessageBox::warning(this,
                                     tr("Can't write local data"),
                                     tr("Writing the local data takes too long, something must be wrong.\n\n"
                                        "You can restart the application and then try again.\n\n"
                                        "If this happens again, file a bug report.")
                                     );
            return;
        }

        // Get User
        RamUser *user = RamUser::get(uuid);
        if (!user)
        {
            QMessageBox::warning(this,
                                     tr("Can't get local user"),
                                     tr("Something went wrong when getting the local user for this database, sorry.\n\n"
                                        "Try again, or file a bug report.")
                                     );
            return;
        }
        // Set its password
        user->updatePassword("", ui_onlinePasswordEdit->text());

        // And finish login
        Ramses::instance()->setUser( user );

        // Hide dock
        MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
        mw->hidePropertiesDock();
    }
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
    mainLayout->setContentsMargins(3, 3, 3, 3);

    ui_fileSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, dummy);
    ui_fileSelector->setPlaceHolderText(tr("File path of the Ramses Database..."));
    ui_fileSelector->setDialogTitle(tr("Select the location of the Ramses Database."));
    ui_fileSelector->setMode(DuQFFolderSelectorWidget::Save);
    ui_fileSelector->setFilter(tr("Ramses (*.ramses);;SQLite (*.sqlite);;All Files (*.*)"));
    mainLayout->addWidget(ui_fileSelector);

    ui_tabWidget = new QTabWidget(dummy);
    ui_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainLayout->addWidget(ui_tabWidget);

    QWidget *offlineWidget = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(offlineWidget, QIcon(":/icons/folder"), tr("Offline"));
    QGridLayout *offlineLayout = new QGridLayout(offlineWidget);
    offlineLayout->setAlignment(Qt::AlignTop);
    offlineLayout->setSpacing(3);
    offlineLayout->setContentsMargins(3,3,3,3);

    QLabel *userLabel = new QLabel("User ID", dummy);
    offlineLayout->addWidget(userLabel, 0, 0);

    ui_shortNameEdit = new QLineEdit(dummy);
    offlineLayout->addWidget(ui_shortNameEdit, 0, 1);

    QLabel *newPasswordLabel = new QLabel(tr("New password"), this);
    offlineLayout->addWidget(newPasswordLabel, 1, 0);

    ui_npassword1Edit = new QLineEdit(this);
    ui_npassword1Edit->setEchoMode(QLineEdit::Password);
    offlineLayout->addWidget(ui_npassword1Edit, 1, 1);

    QLabel *newPasswordLabel2 = new QLabel(tr("Repeat new password"), this);
    offlineLayout->addWidget(newPasswordLabel2, 2, 0);

    ui_npassword2Edit = new QLineEdit(this);
    ui_npassword2Edit->setEchoMode(QLineEdit::Password);
    offlineLayout->addWidget(ui_npassword2Edit, 2, 1);

    QWidget *onlineWidget = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(onlineWidget, QIcon(":/icons/server-settings"), tr("Online (Sync)"));
    QGridLayout *onlineLayout = new QGridLayout(onlineWidget);
    onlineLayout->setAlignment(Qt::AlignTop);
    onlineLayout->setSpacing(3);
    onlineLayout->setContentsMargins(3,3,3,3);
    onlineLayout->setAlignment(Qt::AlignTop);

    onlineLayout->addWidget(new QLabel(tr("Server")), 0, 0);
    ui_serverEdit = new ServerEditWidget();
    onlineLayout->addWidget(ui_serverEdit, 0, 1);

    QLabel *onlineUserLabel = new QLabel("User ID");
    onlineLayout->addWidget(onlineUserLabel, 1, 0);

    ui_onlineShortNameEdit = new QLineEdit();
    onlineLayout->addWidget(ui_onlineShortNameEdit, 1, 1);

    QLabel *onlineNewPasswordLabel = new QLabel(tr("Password"));
    onlineLayout->addWidget(onlineNewPasswordLabel, 2, 0);

    ui_onlinePasswordEdit = new QLineEdit();
    ui_onlinePasswordEdit->setEchoMode(QLineEdit::Password);
    onlineLayout->addWidget(ui_onlinePasswordEdit, 2, 1);

    ui_createButton = new QPushButton(tr("Create and log in"));
    ui_createButton->setIcon(QIcon(":/icons/apply"));
    mainLayout->addWidget(ui_createButton);

    mainLayout->addStretch();

#ifdef QT_DEBUG
    ui_fileSelector->setPath("/home/duduf/Documents/test.ramses");
    ui_shortNameEdit->setText("Duf");
    ui_npassword1Edit->setText("pass");
    ui_npassword2Edit->setText("pass");

    ui_serverEdit->setAddress("ramses.rxlab.io/tests");
    ui_onlineShortNameEdit->setText("Admin");
    ui_onlinePasswordEdit->setText("password");
#endif
}

void DatabaseCreateWidget::connectEvents()
{
    connect(ui_fileSelector, &DuQFFolderSelectorWidget::pathChanged, this, &DatabaseCreateWidget::checkPath);
    connect(ui_createButton, &QPushButton::clicked, this, &DatabaseCreateWidget::createDB);
}

void DatabaseCreateWidget::createNewDB()
{
    // Remove existing file
    QString newFilePath = ui_fileSelector->path();
    if (QFileInfo::exists(newFilePath))
    {
        QMessageBox::StandardButton ok = QMessageBox::question(this, tr("Confirm file overwrite"), tr("Are you sure you want to overwrite this file?") + "\n\n" + newFilePath);
        if (ok != QMessageBox::Yes) return;
        FileUtils::remove(newFilePath);
    }

    // Copy the file
    FileUtils::copy(":/data/template", newFilePath);

    if (!QFileInfo::exists(newFilePath))
    {
        QMessageBox::warning(this, tr("I can't save the database"), tr("I'm sorry, I've failed to create the database at this location.\nMaybe you can try another location...") + "\n\n" + newFilePath );
        return;
    }

    // Set File
    DBInterface::instance()->setDataFile(newFilePath);
}
