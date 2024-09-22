#include "landingpagewidget.h"

#include <QLabel>
#include <QFileInfo>
#include <QtDebug>
#include <QFileDialog>
#include <QDesktopServices>

#include "duapp/app-version.h"
#include "duapp/duui.h"
#include "duwidgets/duicon.h"
#include "duwidgets/dulogindialog.h"
#include "statemanager.h"
#include "wizards/jointeamprojectwizard.h"
#include "wizards/projectwizard.h"
#include "dbinterface.h"
#include "ramses.h"
#include "ramserverclient.h"
#include "ramsettings.h"
#include "datacrypto.h"
#include "wizards/serverwizard.h"

LandingPageWidget::LandingPageWidget(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
    updateRecentList();
}

void LandingPageWidget::updateRecentList()
{
    ui_recentBox->clear();
    const QStringList projects = DBInterface::recentDatabases();

    for(const auto &project: projects) {
        QFileInfo f(project);
        ui_recentBox->addItem(f.completeBaseName(), project);
    }
}

void LandingPageWidget::createDatabase(bool team)
{
    showWizard(new ProjectWizard(team));
}

void LandingPageWidget::openDatabase(const QString &dbFile)
{
    auto sm = StateManager::i();
    sm->setTitle( tr("Opening database: %1").arg(dbFile) );

    // If this is a team project, login
    bool teamProject = DBInterface::isTeamProject(dbFile);
    if (teamProject) {

        QString userUuid = "";
        ServerConfig serverConfig = LocalDataInterface::getServerSettings(dbFile);
        QString serverAddressSettings = serverConfig.address;
        serverAddressSettings.replace("/", "|");

        auto crypto = DataCrypto::instance();

        // Get saved credentials
        QString savedUsername = DuSettings::i()->get(
            RamSettings::Login,
            "",
            serverAddressSettings + "/username"
            ).toString();
        savedUsername = crypto->machineDecrypt(savedUsername);

        QString savedPassword = DuSettings::i()->get(
           RamSettings::Login,
           "",
           serverAddressSettings + "/password"
           ).toString();
        savedPassword = crypto->machineDecrypt(savedPassword);

        // Try to login with saved credentials
        if (savedUsername != "" && savedPassword != "") {
            userUuid = login(
                serverConfig,
                savedUsername,
                savedPassword,
                true
                );
        }
        // Erase the saved password
        savedPassword = "";

        while (userUuid == "") {

            DuLoginDialog d("E-mail");
            d.setUsername(savedUsername);
            int r = DuUI::execDialog(&d);

            if (r == -1) { // Reset password
                QJsonObject rep = RamServerClient::i()->resetPasswordWithEmail(
                    d.username()
                    );

                if (!rep.value("success").toBool(false)) {
                    QMessageBox::warning(this,
                                         tr("Failed to reset password"),
                                         rep.value("message").toString("Unknown error")
                                         );
                }
                else {
                    QMessageBox::information(this,
                                             tr("Password changed"),
                                             rep.value("message").toString("Password successfully changed!")
                                             );
                }
                return;
            }

            if (!r)
                return;

            userUuid = login(
                serverConfig,
                d.username(),
                d.password()
                );

            if (userUuid == "") { // Ask to continue in offline mode
                if (QMessageBox::question(
                    this,
                    tr("Continue in offline mode"),
                    tr("Login failed. Do you want to continue in offline mode?\n\n"
                       "The data will not be synced until you login again, but you can still work locally." )
                    ) != QMessageBox::Yes )
                    continue;
                else
                    break;
            }

            // Save username
            if (d.saveUsername()) {
                DuSettings::i()->set(
                    RamSettings::Login,
                    crypto->machineEncrypt(d.username()),
                    serverAddressSettings+ "/username"
                    );
                if (d.savePassword())
                    DuSettings::i()->set(
                        RamSettings::Login,
                        crypto->machineEncrypt(d.password()),
                        serverAddressSettings + "/password"
                        );
                else // Erase
                    DuSettings::i()->set(
                        RamSettings::Login,
                        "",
                        serverAddressSettings + "/password"
                        );
            }
            else // Erase
                DuSettings::i()->set(
                    RamSettings::Login,
                    "",
                    serverAddressSettings + "/username"
                    );
        }

        // Set current project if we're online

        if (userUuid != "") {
            StateChanger s(StateManager::Opening);

            QString projectUuid = LocalDataInterface::projectUuid(dbFile);
            if (projectUuid == "") {
                QMessageBox::warning(this,
                                     tr("Login failed"),
                                     tr("Can't get the project UUID.\n"
                                        "The local data may be corrupted or incorrectly initialized.\n"
                                        "Try to remove your *.ramses local file and join the team project again.\n\n"
                                        "If this problem persists, this may be a bug or a misconfiguration,\n"
                                        "Please contact your Ramses administrator.")
                                     );
                return;
            }

            QJsonObject rep = RamServerClient::i()->setProject(projectUuid);
            if (!rep.value("success").toBool(false)) {
                QMessageBox::warning(this,
                                     tr("Login failed"),
                                     rep.value("message").toString("Unknown error")
                                     );
                return;
            }

            // Save the project and user in the database

            LocalDataInterface::setProjectUserUuid(dbFile, projectUuid, userUuid);
        }
    }

    sm->setState(StateManager::Opening);

    qInfo().noquote() << tr("Loading data...");

    DBInterface::i()->loadDataFile(dbFile);

    // Restart sync
    qInfo().noquote() << tr("Initial sync...");
    if (teamProject && RamServerClient::i()->status() != RamServerClient::Offline) {
        sm->autoDisconnect(
            connect(DBInterface::i(), &DBInterface::syncFinished, sm, &StateManager::setIdle)
            );
        DBInterface::i()->fullSync();
    }
    else {
        sm->autoDisconnect(
            connect(Ramses::i(), &Ramses::ready, sm, &StateManager::setIdle)
            );
    }

    qInfo().noquote() << tr("Init Ramses...");
    Ramses::i()->loadDatabase();
}

void LandingPageWidget::joinTeamProject()
{
    showWizard(new JoinTeamProjectWizard());
}

void LandingPageWidget::manageServer()
{
    showWizard(new ServerWizard());
}

void LandingPageWidget::setupUi()
{
    ui_stackedLayout = DuUI::addStackedLayout(this);

    QWidget *landingWidget = new QWidget(this);
    ui_stackedLayout->addWidget(landingWidget);

    auto mainLayout = DuUI::addBoxLayout(Qt::Horizontal, landingWidget);
    mainLayout->addStretch();
    auto centerLayout = DuUI::addBoxLayout(Qt::Vertical, mainLayout);
    mainLayout->addStretch();

    centerLayout->addStretch(2);

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/icons/logo_large"));
    logoLabel->setMinimumSize(QSize(256, 256));
    logoLabel->setMaximumSize(QSize(256, 256));
    logoLabel->setScaledContents(true);
    logoLabel->setAlignment(Qt::AlignCenter);
    centerLayout->addWidget(logoLabel);
    centerLayout->setAlignment(logoLabel, Qt::AlignCenter);

    centerLayout->addStretch(1);

    auto buttonLayout = DuUI::addGridLayout(centerLayout);

    ui_createLocalProjectButton = new QPushButton(tr("Create\nproject..."), this);
    ui_createLocalProjectButton->setIcon(DuIcon(":/icons/ramses-file"));
    ui_createLocalProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_createLocalProjectButton, 0, 0);

    ui_createTeamProjectButton = new QPushButton(tr("Create\nteam project..."), this);
    ui_createTeamProjectButton->setIcon(DuIcon(":/icons/users"));
    ui_createTeamProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_createTeamProjectButton, 0, 1);

    ui_openDatabaseButton = new QPushButton(tr("Open\nproject..."), this);
    ui_openDatabaseButton->setIcon(DuIcon(":/icons/open"));
    ui_openDatabaseButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_openDatabaseButton, 1, 0);

    ui_joinTeamProjectButton = new QPushButton(tr("Join\nteam project..."), this);
    ui_joinTeamProjectButton->setIcon(DuIcon(":/icons/login"));
    ui_joinTeamProjectButton->setIconSize(QSize(32,32));
    buttonLayout->addWidget(ui_joinTeamProjectButton, 1, 1);

    auto recentLayout = DuUI::addBoxLayout(Qt::Horizontal, centerLayout);

    recentLayout->addWidget(
        new QLabel( tr("Open recent:"), this)
        );

    ui_recentBox = new DuComboBox(this);
    recentLayout->addWidget(ui_recentBox);

    ui_openRecentDatabaseButton = new QToolButton(this);
    ui_openRecentDatabaseButton->setIcon(DuIcon(":/icons/check"));
    ui_openRecentDatabaseButton->setEnabled(ui_recentBox->currentIndex() >= 0);
    recentLayout->addWidget(ui_openRecentDatabaseButton);

    recentLayout->setStretch(0, 0);
    recentLayout->setStretch(1, 1);
    recentLayout->setStretch(2, 0);

    centerLayout->addStretch(1);

    auto toolsLayout = DuUI::addBoxLayout(Qt::Horizontal, centerLayout);

    ui_manageServerButton = new QToolButton(this);
    ui_manageServerButton->setText(tr("Server..."));
    ui_manageServerButton->setToolTip(tr("Manage your Ramses server users and projects"));
    ui_manageServerButton->setIcon(DuIcon(":/icons/server"));
    toolsLayout->addWidget(ui_manageServerButton);

    ui_helpButton = new QToolButton(this);
    ui_helpButton->setText(tr("User guide"));
    ui_helpButton->setToolTip(tr("Open the online Ramses user guide."));
    ui_helpButton->setIcon(DuIcon(":/icons/documentation"));
    toolsLayout->addWidget(ui_helpButton);

    ui_donateButton = new QToolButton(this);
    ui_donateButton->setText(tr("Donate"));
    ui_donateButton->setToolTip(tr("Support free and open source software development."));
    ui_donateButton->setIcon(DuIcon(":/icons/donate"));
    toolsLayout->addWidget(ui_donateButton);

    centerLayout->addStretch(2);
}

void LandingPageWidget::connectEvents()
{
    connect(ui_recentBox, QOverload<int>::of( &DuComboBox::currentIndexChanged ),
            this, [this] (int index) {
        ui_openRecentDatabaseButton->setEnabled(index >= 0);
    });

    connect(ui_openRecentDatabaseButton, &QToolButton::clicked,
            this, [this] () {
        openDatabase(ui_recentBox->currentData().toString());
    });

    connect(ui_openDatabaseButton, &QPushButton::clicked,
            this, [this] () {
        QString dbFile = QFileDialog::getOpenFileName(
            this,
            STR_INTERNALNAME,
            "",
            "Ramses Project (*.ramses);;SQLite (*.sqlite);;All Files (*.*)");
        if (dbFile == "") return;

        openDatabase(dbFile);
    });

    connect(ui_createLocalProjectButton, &QPushButton::clicked,
            this, [this] () { createDatabase(false); });
    connect(ui_createTeamProjectButton, &QPushButton::clicked,
            this, [this] () { createDatabase(true); });

    connect(ui_joinTeamProjectButton, &QPushButton::clicked,
            this, &LandingPageWidget::joinTeamProject);

    connect(ui_manageServerButton, &QPushButton::clicked,
            this, &LandingPageWidget::manageServer);

    connect(ui_donateButton, &QToolButton::clicked, this, []() { QDesktopServices::openUrl ( QUrl( URL_DONATION ) ); });
    connect(ui_helpButton, &QToolButton::clicked, this, []() { QDesktopServices::openUrl ( QUrl( URL_DOC ) ); });
}

QString LandingPageWidget::login(ServerConfig serverSettings, const QString &username, const QString &password, bool silentFail)
{
    // Set server settings
    RamServerClient::i()->setServerSettings( serverSettings );

    // Login

    QJsonObject rep = RamServerClient::i()->login( username, password );
    if (!rep.value("success").toBool(false)) {
        if (!silentFail)
            QMessageBox::warning(this,
                                 tr("Login failed"),
                                 rep.value("message").toString("Unknown error")
                                 );
        return "";
    }

    QString userUuid = rep.value("content").toObject().value("uuid").toString();

    if (userUuid == "") {
        if (!silentFail)
            QMessageBox::warning(this,
                                 tr("Login failed"),
                                 tr("Can't get your UUID.\n"
                                    "This is probably a misconfiguration or a bug of the server.\n"
                                    "Please contact your Ramses administrator.")
                                 );
        return "";
    }

    return userUuid;
}

void LandingPageWidget::showWizard(QWizard *w)
{
    // Necessary for the margins
    auto dummy = new QWidget(this);
    auto l = DuUI::addBoxLayout(Qt::Vertical, dummy);
    ui_stackedLayout->addWidget(dummy);

    // Create the wizard and show it
    l->addWidget(w);

    // Delete it when finished
    connect(w, &QWizard::finished, this, [this,w,dummy] () {
        ui_stackedLayout->setCurrentIndex(0);
        w->deleteLater();
        dummy->deleteLater();
    });

    ui_stackedLayout->setCurrentIndex(1);
}
