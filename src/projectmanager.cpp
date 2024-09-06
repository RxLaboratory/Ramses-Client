#include "projectmanager.h"

#include "statemanager.h"
#include "progressmanager.h"
#include "ramsettings.h"
#include "dbinterface.h"

ProjectManager *ProjectManager::_instance = nullptr;

ProjectManager *ProjectManager::i()
{
    if (!_instance) _instance = new ProjectManager();
    return _instance;
}

QStringList ProjectManager::recentDatabases() const
{
    QStringList projects;

    const QVector<QHash<QString,QVariant>> recentList = DuSettings::i()->getArray(RamSettings::RecentProjects);
    for(const auto &recent: recentList) {
        QString p = recent.value("path", "").toString();
        if (QFileInfo::exists(p))
            projects << p;
    }

    return projects;
}

void ProjectManager::openDatabase(const QString &dbPath)
{
    if (!QFileInfo::exists(dbPath))
        return;

    // If there is a current project, just restart the app passing the new project as argument.
    if (_currentDatabase != "") {
        StateManager::i()->restart(true, dbPath);
        return;
    }

    _currentDatabase = dbPath;

    auto pm = ProgressManager::instance();
    pm->setMaximum(-1);
    pm->setTitle(tr("Opening project"));
    pm->setText(tr("Loading database..."));
    pm->freeze(true);

    // Set database
    DBInterface::instance()->setDataFile( dbPath );

    pm->setText(tr("Preparing sync..."));

    // Save the recent list
    addToRecentList(dbPath);

    // Trigger a full sync
    if (RamServerInterface::instance()->isOnline())
        DBInterface::instance()->fullSync();

    pm->setText(tr("Project ready!"));

    emit databaseChanged(dbPath);
    ProgressManager::instance()->freeze(false);
}

void ProjectManager::loadDatabase(const QString &dbPath)
{
    // Set database
    DBInterface::instance()->setDataFile( dbPath, true );
    // Save the recent list
    addToRecentList(dbPath);
}

void ProjectManager::closeDatabase()
{
    // If there is a current project, just restart the app without arguments.
    if (_currentDatabase != "")
        StateManager::i()->restart(true);
}

bool ProjectManager::createDatabase(const QString &filePath, const QString &projectWorkingFolder)
{
    // Copy the file
    FileUtils::copy(":/data/template", filePath);

    if (!QFileInfo::exists(filePath))
        return false;

    // Save Ramses Path
    LocalDataInterface::setRamsesPath(filePath, projectWorkingFolder);

    return true;
}

ProjectManager::ProjectManager(QObject *parent)
    : QObject{parent}
{}

void ProjectManager::addToRecentList(const QString &projectPath)
{
    QFileInfo projectInfo(projectPath);

    QVector<QHash<QString,QVariant>> recentList;

    QHash<QString,QVariant> recentProject = {{ "path", projectPath }};
    recentList << recentProject;

    const QStringList &savedRecent = recentDatabases();
    for (const auto &p: savedRecent) {
        if (  projectInfo == QFileInfo(p) )
            continue;
        QHash<QString,QVariant> recentProject = {{ "path", p }};
        recentList << recentProject;
    }

    DuSettings::i()->setArray(RamSettings::RecentProjects, recentList);
}
