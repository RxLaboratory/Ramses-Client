#include "landingpage.h"

#include <QLabel>
#include <QFileInfo>
#include <QtDebug>
#include <QFileDialog>

#include "duapp/app-version.h"
#include "duapp/duui.h"
#include "duwidgets/duicon.h"
#include "duwidgets/dulogindialog.h"
#include "wizards/jointeamprojectwizard.h"
#include "wizards/projectwizard.h"
#include "dbinterface.h"
#include "ramses.h"
#include "ramserverclient.h"

LandingPage::LandingPage(QWidget *parent)
    : QWidget{parent}
{
    setupUi();
    connectEvents();
    updateRecentList();
}

void LandingPage::updateRecentList()
{
    ui_recentBox->clear();
    const QStringList projects = DBInterface::recentDatabases();

    for(const auto &project: projects) {
        QFileInfo f(project);
        ui_recentBox->addItem(f.completeBaseName(), project);
    }
}

void LandingPage::createDatabase(bool team)
{
    // Necessary for the margins
    auto dummy = new QWidget(this);
    auto l = DuUI::addBoxLayout(Qt::Vertical, dummy);
    ui_stackedLayout->addWidget(dummy);

    // Create a wizard and show it
    auto pw = new ProjectWizard(team);
    l->addWidget(pw);

    // Delete it when finished
    connect(pw, &ProjectWizard::finished, this, [this,pw,dummy] () {
        ui_stackedLayout->setCurrentIndex(0);
        pw->deleteLater();
        dummy->deleteLater();
    });

    ui_stackedLayout->setCurrentIndex(1);
}

void LandingPage::openDatabase(const QString &dbFile)
{
    // If this is a team project, login
    bool teamProject = DBInterface::isTeamProject(dbFile);
    if (teamProject) {

        DuLoginDialog d("E-mail", this);
        if (!d.exec())
            return;

        QString userUuid = login(
            LocalDataInterface::getServerSettings(dbFile),
            d.username(),
            d.password()
            );

        // Set current project

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

    DBInterface::i()->loadDataFile(dbFile);
    // Restart sync
    if (teamProject)
        DBInterface::i()->fullSync();
    Ramses::i()->loadDatabase();
}

void LandingPage::joinTeamProject()
{
    // Necessary for the margins
    auto dummy = new QWidget(this);
    auto l = DuUI::addBoxLayout(Qt::Vertical, dummy);
    ui_stackedLayout->addWidget(dummy);

    // Create a wizard and show it
    auto pw = new JoinTeamProjectWizard();
    l->addWidget(pw);

    // Delete it when finished
    connect(pw, &ProjectWizard::finished, this, [this,pw,dummy] () {
        ui_stackedLayout->setCurrentIndex(0);
        pw->deleteLater();
        dummy->deleteLater();
    });

    ui_stackedLayout->setCurrentIndex(1);
}

void LandingPage::setupUi()
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

    centerLayout->addStretch(2);
}

void LandingPage::connectEvents()
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
            this, &LandingPage::joinTeamProject);
}

QString LandingPage::login(ServerConfig serverSettings, const QString &username, const QString &password)
{
    // Set server settings
    RamServerClient::i()->setServerSettings( serverSettings );

    // Login

    QJsonObject rep = RamServerClient::i()->login( username, password );
    if (!rep.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Login failed"),
                             rep.value("message").toString("Unknown error")
                             );
        return "";
    }

    QString userUuid = rep.value("content").toObject().value("uuid").toString();

    if (userUuid == "") {
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
