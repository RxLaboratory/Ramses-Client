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

QStringList ProjectManager::recentProjects() const
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

void ProjectManager::setProject(const QString &projectPath)
{
    if (!QFileInfo::exists(projectPath))
        return;

    // If there is a current project, just restart the app passing the new project as argument.
    if (_currentProject != "") {
        StateManager::i()->restart(true, projectPath);
        return;
    }

    _currentProject = projectPath;

    auto pm = ProgressManager::instance();
    pm->setMaximum(-1);
    pm->setTitle(tr("Opening project"));
    pm->setText(tr("Loading database..."));
    pm->freeze(true);

    // Set database
    DBInterface::instance()->setDataFile( projectPath );

    pm->setText(tr("Preparing sync..."));

    // Save the recent list
    addToRecentList(projectPath);

    // Trigger a full sync
    if (RamServerInterface::instance()->isOnline())
        DBInterface::instance()->fullSync();

    pm->setText(tr("Project ready!"));

    emit projectChanged(projectPath);
    ProgressManager::instance()->freeze(false);
}

void ProjectManager::closeProject()
{
    // If there is a current project, just restart the app without arguments.
    if (_currentProject != "")
        StateManager::i()->restart(true);
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

    const QStringList &savedRecent = recentProjects();
    for (const auto &p: savedRecent) {
        if (  projectInfo == QFileInfo(p) )
            continue;
        QHash<QString,QVariant> recentProject = {{ "path", p }};
        recentList << recentProject;
    }

    DuSettings::i()->setArray(RamSettings::RecentProjects, recentList);
}
