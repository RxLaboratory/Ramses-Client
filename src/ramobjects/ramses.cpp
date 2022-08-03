#include "ramses.h"

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::instance()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

Ramses::~Ramses()
{
    DBInterface::instance()->suspend(true);
}

Ramses::Ramses(QObject *parent) : RamObject(parent)
{
    qDebug() << "Initialising Ramses";
    m_dbi = DBInterface::instance();

    m_users = new RamObjectList("USRS", "Users", this);
    m_states = new RamStateList(this);
    m_projects = new RamObjectList("PRJCTS", "Projects", this);
    m_templateSteps = new RamObjectList("TPLTSTPS", "Template steps", this);
    m_templateAssetGroups = new RamObjectList("TPLTAGS", "Template asset groups", this);
    m_fileTypes = new RamObjectList ("FLTPS", "File types", this);
    m_applications = new RamObjectList("APPS", "Applications", this);
    m_ramUser = nullptr;

    DBISuspender s;

    m_currentUser = nullptr;
    m_currentProject = nullptr;
    m_ramsesPath = m_settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString();

    m_connected = false;

    connect( (DuApplication *)qApp, &DuApplication::idle, this, &Ramses::refresh);
    connect( m_dbi, &DBInterface::connectionStatusChanged, this, &Ramses::dbiConnectionStatusChanged);

    this->setObjectName( "Ramses Class" );

    qDebug() << "Ramses Ready!";
}

void Ramses::login(QString username, QString password)
{
    m_dbi->suspend(false);
    // Hash password and post
    password = m_dbi->generatePassHash(password);
    QJsonObject data;
    data.insert("username", username);
    data.insert("password", password);
    m_dbi->post("login", data);
}

void Ramses::loginHashed(QString username, QString hashedPassword)
{
    m_dbi->suspend(false);
    m_dbi->loginHashed(username, hashedPassword);
}

void Ramses::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s, QString reason)
{
    if (s == NetworkUtils::Offline)
    {
        QSignalBlocker b(m_dbi);
        logout(reason);
    }
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

void Ramses::setRamsesPath(const QString &ramsesPath)
{
    m_ramsesPath = ramsesPath;
}

QString Ramses::pathFromRamses(QString p, bool create) const
{
    QString path;
    if (QFileInfo( p ).isRelative())
    {
        path = m_ramsesPath + "/" + p;
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

void Ramses::setCurrentUser(RamUser *u)
{
    if (u) m_connected = true;
    else m_connected = false;

    if (!m_currentUser && !u) return;
    if (m_currentUser) if (m_currentUser->is(u)) return;

    m_currentUser = u;

    if (u) emit loggedIn(m_currentUser);
    else
    {
        setCurrentProject(nullptr);
        emit loggedOut("No current user");
    }
}

RamProject *Ramses::currentProject() const
{
    return m_currentProject;
}

void Ramses::init()
{
    if(!m_currentUser) return;
    QSettings *uSettings = m_currentUser->settings();
    setCurrentProjectUuid(uSettings->value("ramses/currentProject", "").toString());
}

void Ramses::setCurrentProject(RamProject *project)
{

    if ( m_currentProject ) if ( m_currentProject->is(project) ) return;
    if ( project )
    {
        qDebug() << "Setting project: Database call";
        m_dbi->getProject(project->uuid());
    }
    else
    {
        qDebug() << "Setting project to none";
        m_currentProject = nullptr;
        emit currentProjectChanged(m_currentProject);
    }
}

void Ramses::projectReady(QString uuid)
{
    RamProject *currentProject = RamProject::getObject(uuid);
    if ( !currentProject ) return;

    if (currentProject->is( m_currentProject )) return;

    m_currentProject = currentProject;
    if(m_currentUser)
    {
        QSettings *uSettings = m_currentUser->settings();
        uSettings->setValue("ramses/currentProject", m_currentProject->uuid() );
    }

    qDebug() << "Setting current project to: " + m_currentProject->shortName();
#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif
    emit currentProjectChanged(m_currentProject);
}

void Ramses::setOnline()
{
    m_connected = true;
}

void Ramses::setOffline()
{
    m_connected = false;
}

QString Ramses::folderPath() const
{
    return m_ramsesPath;
}

void Ramses::setCurrentProject(QString shortName)
{
    setCurrentProject( RamProject::objectFromName(shortName) );
}

void Ramses::setCurrentProjectUuid(QString uuid)
{
    setCurrentProject( RamProject::getObject(uuid) );
}

RamObjectList *Ramses::templateSteps() const
{
    return m_templateSteps;
}

RamObjectList *Ramses::templateAssetGroups() const
{
    return m_templateAssetGroups;
}

RamStateList *Ramses::states() const
{
    return m_states;
}

RamObjectList *Ramses::fileTypes() const
{
    return m_fileTypes;
}

RamObjectList *Ramses::applications() const
{
    return m_applications;
}

RamObjectList *Ramses::projects() const
{
    return m_projects;
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

RamState *Ramses::noState()
{
    if(m_noState) return m_noState;
    m_noState = qobject_cast<RamState*>( m_states->fromName("NO") );
    if (!m_noState)
    {
        m_noState = new RamState("NO", "Nothing to do");
        m_noState->setColor(QColor(36,36,36));
        m_states->append(m_noState);
    }
    return m_noState;
}

RamState *Ramses::todoState()
{
    if(m_todoState) return m_todoState;
    m_todoState =  qobject_cast<RamState*>( m_states->fromName("TODO") );
    if (!m_todoState)
    {
        m_todoState = new RamState("TODO", "To do");
        m_todoState->setColor(QColor(85,170,255));
        m_states->append(m_todoState);
    }
    return m_todoState;
}

RamState *Ramses::okState()
{
    if(m_okState) return m_okState;
    m_okState =  qobject_cast<RamState*>( m_states->fromName("OK") );
    if (!m_okState)
    {
        m_okState = new RamState("OK", "Finished");
        m_okState->setColor(QColor(0,170,0));
        m_states->append(m_okState);
    }
    return m_okState;
}

RamState *Ramses::stbState()
{
    if(m_stbState) return m_stbState;
    m_stbState = qobject_cast<RamState*>( m_states->fromName("STB") );
    if (!m_stbState)
    {
        m_stbState = new RamState("STB", "Stand by");
        m_stbState->setColor(QColor(168,168,168));
        m_states->append(m_stbState);
    }
    return m_stbState;
}

RamState *Ramses::wipState()
{
    if (m_wipState) return m_wipState;
    m_wipState = qobject_cast<RamState*>( m_states->fromName("WIP") );
    if (!m_wipState)
    {
        m_wipState = new RamState("WIP", "Work in progress");
        m_wipState->setColor(QColor(255,255,127));
        m_states->append(m_wipState);
    }
    return m_wipState;
}

void Ramses::logout(QString reason)
{
    setOffline();
    // Suspend the db interface until there's a login!
    m_dbi->suspend();

    setCurrentProject(nullptr);
    m_currentUser = nullptr;

    emit loggedOut(reason);
}

void Ramses::refresh()
{
    /*
    // Get Users
    _dbi->getUsers();
    // Get Template Steps
    _dbi->getTemplateSteps();
    // Get Template Asset Groups
    _dbi->getTemplateAssetGroups();
    // Get States
    _dbi->getStates();
    // Get file types
    _dbi->getFileTypes();
    // Get applications
    _dbi->getApplications();*/

    // Only if connected
    if (!isOnline()) return;

    // Get current project
    m_dbi->post("init");
}

bool Ramses::isOnline() const
{
    return m_connected;
}

RamUser *Ramses::currentUser() const
{
    return m_currentUser;
}

RamUser *Ramses::ramUser()
{
    if (m_ramUser) return m_ramUser;
    m_ramUser = qobject_cast<RamUser*>( m_users->fromName("Ramses") );
    if (!m_ramUser)
    {
        m_ramUser = new RamUser("Ramses", "Ramses Daemon");
        m_ramUser->updatePassword("", RamUuid::generateUuidString("Ramses"));
    }
    return m_ramUser;
}

RamUser *Ramses::removedUser()
{
    if (m_removedUser) return m_removedUser;
    m_removedUser = qobject_cast<RamUser*>( m_users->fromName("Removed") );
    if (!m_removedUser)
    {
        m_removedUser = new RamUser("Removed", "Removed User");
        m_removedUser->updatePassword("", RamUuid::generateUuidString("Removed"));
    }
    return m_removedUser;
}

void Ramses::setRamUser(RamUser *user)
{
    m_ramUser = user;
}

RamObjectList *Ramses::users() const
{
    return m_users;
}
