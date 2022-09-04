#include "ramses.h"

// STATIC //

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::instance()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

void Ramses::setUserUuid(QString uuid)
{
    if (uuid == "")
    {
        setUser(nullptr);
        return;
    }
    RamUser *u = RamUser::get(uuid);
    setUser(u);
}

// PUBLIC

void Ramses::setUser(RamUser *u)
{
    m_currentUser = u;
    if (!u)
    {
        setCurrentProject(nullptr);
        emit userChanged(nullptr);
        qDebug() << "Logged out.";
        return;
    }

    // Set current project
    QSettings *uSettings = m_currentUser->settings();
    QString projUuid = uSettings->value("ramses/currentProject", "").toString();

    if (projUuid != "") setCurrentProject( RamProject::get(projUuid) );
    else setCurrentProject(nullptr);

    m_dbi->setCurrentUserUuid(m_currentUser->uuid());

    emit userChanged(m_currentUser);

    qDebug() << "Logged in: " + m_currentUser->name();
}

void Ramses::setRamsesPath(QString p)
{
    m_dbi->setRamsesPath(p);
}

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
        return createPath( p );
    else
        return p;
}

RamObjectList *Ramses::users() const
{
    return m_users;
}

RamUser *Ramses::currentUser() const
{
    return m_currentUser;
}

RamUser *Ramses::ramsesUser()
{
    if (m_ramsesUser) return m_ramsesUser;

    m_ramsesUser = RamUser::c( m_users->fromName("Ramses") );

    if (!m_ramsesUser)
    {
        m_ramsesUser = new RamUser("Ramses", "Ramses Daemon");
        m_users->append(m_ramsesUser);
    }
    return m_ramsesUser;
}

RamUser *Ramses::removedUser()
{
    if (m_removedUser) return m_removedUser;

    m_removedUser = RamUser::c( m_users->fromName("Removed") );
    if (!m_removedUser)
    {
        m_removedUser = new RamUser("Removed", "Removed User");
    }
    return m_removedUser;
}

bool Ramses::isAdmin()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Admin;
}

bool Ramses::isProjectAdmin()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::ProjectAdmin;
}

bool Ramses::isLead()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Lead;
}

DBTableModel *Ramses::states() const
{
    return m_states;
}

RamState *Ramses::noState()
{
    if (m_noState) return m_noState;
    m_noState = RamState::c( m_states->search("NO") );
    if (!m_noState)
    {
        m_noState = new RamState("NO", "Nothing to do");
        m_noState->setColor(QColor(36,36,36));
        m_states->insertObjects(
                    m_states->rowCount(),
                    QStringList(m_noState->uuid())
                    );
    }
    return m_noState;
}

RamState *Ramses::todoState()
{
    if(m_todoState) return m_todoState;
    m_todoState =  RamState::c( m_states->search("TODO") );
    if (!m_todoState)
    {
        m_todoState = new RamState("TODO", "To do");
        m_todoState->setColor(QColor(85,170,255));
        m_states->insertObjects(
                    m_states->rowCount(),
                    QStringList(m_todoState->uuid())
                    );
    }
    return m_todoState;
}

RamState *Ramses::okState()
{
    if(m_okState) return m_okState;
    m_okState =  RamState::c( m_states->search("OK") );
    if (!m_okState)
    {
        m_okState = new RamState("OK", "Finished");
        m_okState->setColor(QColor(0,170,0));
        m_states->insertObjects(
                    m_states->rowCount(),
                    QStringList(m_okState->uuid())
                    );
    }
    return m_okState;
}

RamState *Ramses::stbState()
{
    if(m_stbState) return m_stbState;
    m_stbState = RamState::c( m_states->search("STB") );
    if (!m_stbState)
    {
        m_stbState = new RamState("STB", "Stand by");
        m_stbState->setColor(QColor(168,168,168));
        m_states->insertObjects(
                    m_states->rowCount(),
                    QStringList(m_stbState->uuid())
                    );
    }
    return m_stbState;
}

RamState *Ramses::wipState()
{
    if (m_wipState) return m_wipState;
    m_wipState = RamState::c( m_states->search("WIP") );
    if (!m_wipState)
    {
        m_wipState = new RamState("WIP", "Work in progress");
        m_wipState->setColor(QColor(255,255,127));
        m_states->insertObjects(
                    m_states->rowCount(),
                    QStringList(m_wipState->uuid())
                    );
    }
    return m_wipState;
}

RamObjectList *Ramses::projects() const
{
    return m_projects;
}

RamProject *Ramses::currentProject() const
{
    return m_currentProject;
}

void Ramses::setCurrentProject(RamProject *project)
{
    m_currentProject = project;

    if (m_currentUser && m_currentProject)
    {
        QSettings *uSettings = m_currentUser->settings();
        uSettings->setValue("ramses/currentProject", m_currentProject->uuid() );
    }
    emit currentProjectChanged(m_currentProject);
}

void Ramses::setCurrentProject(QString shortName)
{
    setCurrentProject( RamProject::c(m_projects->fromName(shortName)) );
}

void Ramses::setCurrentProjectUuid(QString uuid)
{
    if (uuid == "") setCurrentProject( nullptr );
    setCurrentProject( RamProject::get(uuid) );
}

RamObjectList *Ramses::templateSteps() const
{
    return m_templateSteps;
}

RamObjectList *Ramses::templateAssetGroups() const
{
    return m_templateAssetGroups;
}

RamObjectList *Ramses::fileTypes() const
{
    return m_fileTypes;
}

RamObjectList *Ramses::applications() const
{
    return m_applications;
}

// PUBLIC SLOTS

void Ramses::refresh()
{
    if (!m_loggedin) return;

    // (Re)Load admin data

    m_users->reload();
    m_templateSteps->reload();
    m_templateAssetGroups->reload();
    m_states->reload();
    m_fileTypes->reload();
    m_applications->reload();
    m_projects->reload();
}

// PROTECTED

QString Ramses::folderPath() const
{
    QString path = m_dbi->ramsesPath();
    if (path == "" || path == "auto") path = QDir::homePath() + "/Ramses";
    return path;
}

// PRIVATE

Ramses::Ramses(QObject *parent):
    RamObject("RMSS", "Ramses", ObjectType::Ramses, parent, true)
{
    qDebug() << "Initialising Ramses";
    m_dbi = DBInterface::instance();

    m_states = new DBTableModel(RamAbstractObject::State, this);

    m_users = new RamObjectList("RamUser", "Users", User, RamObjectList::Table, this);
    m_projects = new RamObjectList("RamProject", "Projects", Project, RamObjectList::Table, this);
    m_templateSteps = new RamObjectList("RamTemplateStep", "Template steps", TemplateStep, RamObjectList::Table, this);
    m_templateAssetGroups = new RamObjectList("RamTemplateAssetGroup", "Template asset groups", TemplateAssetGroup, RamObjectList::Table, this);
    m_fileTypes = new RamObjectList("RamFileType", "File types", FileType, RamObjectList::Table, this);
    m_applications = new RamObjectList("RamApplication", "Applications", Application, RamObjectList::Table, this);

    this->setObjectName( "Ramses Class" );

    connect(m_dbi, &DBInterface::userChanged, this, &Ramses::setUserUuid);

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



































