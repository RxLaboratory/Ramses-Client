#include "jointeamprojectwizard.h"

#include <QMessageBox>

#include "duutils/utils.h"
#include "dbinterface.h"
#include "ramproject.h"
#include "ramserverclient.h"
#include "ramuser.h"
#include "ramses.h"

JoinTeamProjectWizard::JoinTeamProjectWizard(QWidget *parent, Qt::WindowFlags flags):
    QWizard(parent, flags)
{
    setupUi();
    connectEvents();
}

void JoinTeamProjectWizard::done(int r)
{
    if (r != QWizard::Accepted)
        return QWizard::done(r);

    // Create the local database
    QString dbPath = ui_pathsPage->dbFilePath();

    // Remove existing file
    if (!askRemoveExistingFile(dbPath))
        return;

    // Create the file
    if (!createDatabase(dbPath))
        return;

    // Disable while we finish the setup
    this->setEnabled(false);

    // Set the file
    DBInterface::i()->loadDataFile(dbPath);
    // Set the server settings in the DB
    LocalDataInterface::instance()->setServerSettings(dbPath, RamServerClient::i()->serverConfig());
    // Download distant data
    RamServerClient::i()->setProject(ui_projectsPage->uuid());
    // Just wait for it
    connect(DBInterface::i(), &DBInterface::syncFinished,
            this, &JoinTeamProjectWizard::finishProjectSetup);
    RamServerClient::i()->downloadAllData();
    return;
}

void JoinTeamProjectWizard::finishProjectSetup()
{
    disconnect(DBInterface::i(), nullptr, this, nullptr);

    QString projectUuid = ui_projectsPage->uuid();
    QString userUuid = ui_loginPage->uuid();

    RamProject *project = RamProject::get(projectUuid);
    if (!project) {
        QMessageBox::warning(
            this,
            tr("Project creation failed."),
            tr("Something went wrong while downloading data from the server, sorry.")
            );
        this->setEnabled(true);
        return;
    }

    RamUser *user = RamUser::get(userUuid);
    if (!user) {
        QMessageBox::warning(
            this,
            tr("Project creation failed."),
            tr("Something went wrong while downloading data from the server, sorry.")
            );
        this->setEnabled(true);
        return;
    }

    // Set the project in the DB
    LocalDataInterface::instance()->setCurrentProjectUser(projectUuid, userUuid);

    // And tell Ramses we're ready
    Ramses::i()->loadDatabase();

    // and accept
    this->setEnabled(true);
    QWizard::done(QWizard::Accepted);
}

void JoinTeamProjectWizard::setupUi()
{
    setWizardStyle(QWizard::ClassicStyle);

    // Login
    ui_loginPage = new LoginWizardPage(false, this);
    setPage(LoginPage, ui_loginPage );

    // Select project
    ui_projectsPage = new ProjectListWizardPage(this);
    setPage(ProjectPage, ui_projectsPage);

    // DB and working paths
    ui_pathsPage = new RamDatabasePathsWizardPage(this);
    setPage( PathsPage, ui_pathsPage );
}

void JoinTeamProjectWizard::connectEvents()
{

}

bool JoinTeamProjectWizard::askRemoveExistingFile(const QString &dbPath)
{
    if (QFileInfo::exists(dbPath)) {
        QMessageBox::StandardButton ok = QMessageBox::question(
            this,
            tr("Confirm file overwrite"),
            tr("Are you sure you want to overwrite this file?") + "\n\n" + dbPath
            );
        if (ok != QMessageBox::Yes)
            return false;

        FileUtils::remove(dbPath);
    }
    return true;
}

bool JoinTeamProjectWizard::createDatabase(const QString &dbPath)
{
    if (!LocalDataInterface::createNewDatabase( dbPath ) )
    {
        QMessageBox::warning(this,
                             tr("Can't save the database"),
                             tr("Sorry, the database creation failed at this location.\nMaybe you can try another location...") + "\n\n" +
                                 dbPath
                             );
        return false;
    }
    LocalDataInterface::setWorkingPath( dbPath, ui_pathsPage->projectPath() );

    return true;
}

