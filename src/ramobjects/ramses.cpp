#include "ramses.h"

// STATIC //

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::i()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

// PUBLIC

QString Ramses::pathFromRamses(QString p, bool create) const
{
    if (p == "") return folderPath();

    QString path;
    if (QFileInfo( p ).isRelative())
    {
        path = folderPath() + "/" + p;
    }
    else
    {
        path = p;
    }
    if (create)
        return QDir::cleanPath(createPath( p ));
    else
        return QDir::cleanPath(p);
}

DBTableModel *Ramses::users() const
{
    m_users->load();
    return m_users;
}

RamUser *Ramses::currentUser() const
{
    return m_currentUser;
}

RamUser *Ramses::ramsesUser()
{
    m_users->load();
    if (m_ramsesUser) return m_ramsesUser;

    m_ramsesUser = RamUser::c( m_users->search("Ramses") );

    if (!m_ramsesUser)
    {
        m_ramsesUser = new RamUser("Ramses", "Ramses Daemon");
        //m_users->append(m_ramsesUser);
    }
    return m_ramsesUser;
}

RamUser *Ramses::removedUser()
{
    m_users->load();
    if (m_removedUser) return m_removedUser;

    m_removedUser = RamUser::c( m_users->search("Removed") );
    if (!m_removedUser)
    {
        m_removedUser = new RamUser("Removed", "Removed User");
    }
    return m_removedUser;
}

bool Ramses::isAdmin()
{
    if (!DBInterface::i()->isTeamProject())
        return true;

    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Admin;
}

bool Ramses::isProjectAdmin()
{
    if (!DBInterface::i()->isTeamProject())
        return true;

    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::ProjectAdmin;
}

bool Ramses::isLead()
{
    if (!DBInterface::i()->isTeamProject())
        return true;

    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Lead;
}

DBTableModel *Ramses::states() const
{
    m_states->load();
    return m_states;
}

RamState *Ramses::noState()
{
    m_states->load();
    if (m_noState) return m_noState;
    m_noState = RamState::c( m_states->search("NO") );
    if (!m_noState)
    {
        m_noState = new RamState("NO", "Nothing to do");
        m_noState->setColor(QColor(36,36,36));
    }
    return m_noState;
}

RamState *Ramses::todoState()
{
    m_states->load();
    if(m_todoState) return m_todoState;
    m_todoState =  RamState::c( m_states->search("TODO") );
    if (!m_todoState)
    {
        m_todoState = new RamState("TODO", "To do");
        m_todoState->setColor(QColor(85,170,255));
        /*m_states->insertObjects(
                    m_states->rowCount(),
                    QVector<QString>() << m_todoState->uuid()
                    );*/
    }
    return m_todoState;
}

RamState *Ramses::okState()
{
    m_states->load();
    if(m_okState) return m_okState;
    m_okState =  RamState::c( m_states->search("OK") );
    if (!m_okState)
    {
        m_okState = new RamState("OK", "Finished");
        m_okState->setColor(QColor(0,170,0));
        /*m_states->insertObjects(
                    m_states->rowCount(),
                    QVector<QString>() << m_okState->uuid()
                    );*/
    }
    return m_okState;
}

RamState *Ramses::stbState()
{
    m_states->load();
    if(m_stbState) return m_stbState;
    m_stbState = RamState::c( m_states->search("STB") );
    if (!m_stbState)
    {
        m_stbState = new RamState("STB", "Stand by");
        m_stbState->setColor(QColor(168,168,168));
        /*m_states->insertObjects(
                    m_states->rowCount(),
                    QVector<QString>() << m_stbState->uuid()
                    );*/
    }
    return m_stbState;
}

RamState *Ramses::wipState()
{
    m_states->load();
    if (m_wipState) return m_wipState;
    m_wipState = RamState::c( m_states->search("WIP") );
    if (!m_wipState)
    {
        m_wipState = new RamState("WIP", "Work in progress");
        m_wipState->setColor(QColor(255,255,127));
        /*m_states->insertObjects(
                    m_states->rowCount(),
                    QVector<QString>() << m_wipState->uuid()
                    );*/
    }
    return m_wipState;
}

DBTableModel *Ramses::projects() const
{
    m_projects->load();
    return m_projects;
}

RamProject *Ramses::project() const
{
    return m_currentProject;
}

DBTableModel *Ramses::templateSteps() const
{
    m_templateSteps->load();
    return m_templateSteps;
}

DBTableModel *Ramses::templateAssetGroups() const
{
    m_templateAssetGroups->load();
    return m_templateAssetGroups;
}

DBTableModel *Ramses::fileTypes() const
{
    m_fileTypes->load();
    return m_fileTypes;
}

DBTableModel *Ramses::applications() const
{
    m_applications->load();
    return m_applications;
}

// PROTECTED

QString Ramses::folderPath() const
{
    QString path = LocalDataInterface::instance()->workingPath();
    if (path == "" || path == "auto") path = QDir::homePath() + "/Ramses";
    return path;
}

// PRIVATE

Ramses::Ramses(QObject *parent):
    RamObject("RMSS", "Ramses", ObjectType::Ramses, parent, true)
{
    qDebug() << "Initialising Ramses";
    m_dbi = DBInterface::i();

    m_applications = new DBTableModel(RamObject::Application, false, false, this);
    m_fileTypes = new DBTableModel(RamObject::FileType, false, false, this);
    m_projects = new DBTableModel(RamObject::Project, true, false, this);
    m_states = new DBTableModel(RamObject::State, false, false, this);
    m_templateAssetGroups = new DBTableModel(RamObject::TemplateAssetGroup, false, true, this);
    m_templateSteps = new DBTableModel(RamObject::TemplateStep, false, true, this);
    m_users = new DBTableModel(RamObject::User, false, false, this);

    this->setObjectName( "Ramses Class" );

    connect(m_dbi, &DBInterface::databaseReady, this, &Ramses::loadDatabase);

    qDebug() << "Ramses Ready!";
}

QDir Ramses::createDir(QString p) const
{
    QDir d(p);
    if (p != "") d.mkpath(".");
    return d;
}

QString Ramses::createPath(QString p) const
{
    QDir d = createDir(p);
    return d.absolutePath();
}

void Ramses::loadDatabase()
{
    QString userUuid = LocalDataInterface::instance()->currentUserUuid();
    auto user = RamUser::get(userUuid);
    if (!user) return;

    m_currentUser = user;

    // Set current project
    QString projectUuid = LocalDataInterface::instance()->currentProjectUuid();
    auto project = RamProject::get(projectUuid);
    if (!project) return;

    m_currentProject = project;

    qInfo().noquote() << "{Ramses}" << "Logged in: " + m_currentUser->name();
    qInfo().noquote() << "{Ramses}" << "Project: " + m_currentProject->name();

    DBInterface::addToRecentList(
        LocalDataInterface::instance()->dataFile()
        );

    emit ready();
    emit roleChanged(user->role());
}



































