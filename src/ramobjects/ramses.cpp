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

Ramses::Ramses(QObject *parent) : QObject(parent)
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
    m_dbi->login(username, password);
}

void Ramses::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s != NetworkUtils::Online)
    {
        QSignalBlocker b(m_dbi);
        logout();
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

QString Ramses::ramsesPath() const
{
    return createPath( m_ramsesPath );
}

void Ramses::setRamsesPath(const QString &ramsesPath)
{
    m_ramsesPath = ramsesPath;
}

QString Ramses::pathFromRamses(QString p) const
{
    QString path;
    if (QFileInfo( p ).isRelative())
    {
        path = ramsesPath() + "/" +p;
    }
    else
    {
        path = p;
    }
    return createPath( p );
}

QString Ramses::usersPath() const
{
    return createPath( ramsesPath() + "/Users" );
}

QString Ramses::path(RamUser *u) const
{
    if (!u) return pathFromRamses("");

    QString path = u->folderPath();
    if (path == "" || path == "auto") path = usersPath() + "/" + u->shortName();
    return pathFromRamses( path );
}

QDir Ramses::dir(RamUser *u) const
{
    return QDir( path(u) );
}

QString Ramses::currentUserPath() const
{
    RamUser *u = this->currentUser();
    if (!u) return "";
    else return path(u);
}

QDir Ramses::currentUserDir() const
{
    return QDir(currentUserPath());
}

QString Ramses::configPath(RamUser *u) const
{
    return configDir(u).absolutePath();
}

QDir Ramses::configDir(RamUser *u) const
{
    return createDir( path(u) + "/Config" );
}

QString Ramses::currentUserConfigPath() const
{
    RamUser *u = this->currentUser();
    if (!u) return "";
    else return configPath(u);
}

QDir Ramses::currentUserCondigDir() const
{
    return QDir(currentUserConfigPath());
}

QString Ramses::defaultUserPath(RamUser *u) const
{
    return usersPath() + "/" + u->shortName();
}

QString Ramses::projectsPath() const
{
    return createPath( ramsesPath() + "/Projects" );
}

QString Ramses::configPath() const
{
    return createPath( ramsesPath() + "/Config" );
}

QString Ramses::path(RamProject *p) const
{
    if (!p) return pathFromRamses("");

    QString path = p->folderPath();
    if (path == "" || path == "auto") path = projectsPath() + "/" + p->shortName();
    return pathFromRamses( path );
}

QDir Ramses::dir(RamProject *p) const
{
    return QDir(path(p));
}

QString Ramses::configPath(RamProject *p) const
{
    return createPath( path(p) + "/Config" );
}

QDir Ramses::configDir(RamProject *p) const
{
    return QDir(configPath(p));
}

QString Ramses::adminPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "00-ADMIN" );
}

QDir Ramses::adminDir(RamProject *p) const
{
    return QDir(adminPath(p));
}

QString Ramses::preProdPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "01-PRE-PROD" );
}

QDir Ramses::preProdDir(RamProject *p) const
{
    return QDir(preProdPath(p));
}

QString Ramses::prodPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "02-PROD" );
}

QDir Ramses::prodDir(RamProject *p) const
{
    return QDir(prodPath(p));
}

QString Ramses::postProdPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "03-POST-PROD" );
}

QDir Ramses::postProdDir(RamProject *p) const
{
    return QDir(postProdPath(p));
}

QString Ramses::assetsPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "04-ASSETS" );
}

QDir Ramses::assetsDir(RamProject *p) const
{
    return QDir(assetsPath(p));
}

QString Ramses::shotsPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "05-SHOTS" );
}

QDir Ramses::shotsDir(RamProject *p) const
{
    return QDir(shotsPath(p));
}

QString Ramses::exportPath(RamProject *p) const
{
    return createPath( path(p) + "/" + "06-EXPORT" );
}

QDir Ramses::exportDir(RamProject *p) const
{
    return QDir(exportPath(p));
}

QString Ramses::path(RamStep *s) const
{
    RamProject *p = s->project();
    QString path;
    if (!p) return "";
    if (s->type() == RamStep::PreProduction)
    {
        path = preProdPath(p) + "/" + p->shortName() + "_G_" + s->shortName();
    }

    else if (s->type() == RamStep::PostProduction)
    {
        path = postProdPath(p) + "/" + p->shortName() + "_G_" + s->shortName();
    }

    else
    {
        path = prodPath(p) + "/" + p->shortName() + "_G_" + s->shortName();
    }

    return createPath(path);
}

QDir Ramses::dir(RamStep *s) const
{
    return QDir(path(s));
}

QString Ramses::path(RamItem *i) const
{
    if (i->objectType() == RamObject::Asset)
    {
        RamAsset *a = qobject_cast<RamAsset*>( i );
        return assetPath(a);
    }
    if (i->objectType() == RamObject::Shot)
    {
        RamShot *s = qobject_cast<RamShot*>( i );
        return shotPath(s);
    }
    return "";
}

QDir Ramses::dir(RamItem *i) const
{
    if (i->objectType() == RamObject::Asset)
    {
        RamAsset *a = qobject_cast<RamAsset*>( i );
        return assetDir(a);
    }
    if (i->objectType() == RamObject::Shot)
    {
        RamShot *s = qobject_cast<RamShot*>( i );
        return shotDir(s);
    }
    return QDir();
}

QString Ramses::path(RamAssetGroup *ag) const
{
    if(ag->isTemplate()) return "";
    RamProject *p = ag->project();
    if (!p) return "";
    return createPath( assetsPath(p) + "/" + ag->name() );
}

QDir Ramses::dir(RamAssetGroup *ag) const
{
    return QDir(path(ag));
}

QString Ramses::assetPath(RamAsset *a) const
{
    RamAssetGroup *ag = a->assetGroup();
    if (!ag) return "";
    RamProject *p = ag->project();
    if (!p) return "";
    return createPath( path(ag) + "/" + p->shortName() + "_A_" + a->shortName() );
}

QDir Ramses::assetDir(RamAsset *a) const
{
    return QDir(assetPath(a));
}

QString Ramses::shotPath(RamShot *s) const
{
    RamSequence *seq = s->sequence();
    if (!seq) return "";
    RamProject *p = seq->project();
    if (!p) return "";
    return createPath( shotsPath(p) + "/" + p->shortName() + "_S_" + s->shortName() );
}

QDir Ramses::shotDir(RamShot *s) const
{
    return QDir(shotPath(s));
}

QString Ramses::path(RamStatus *s) const
{
    RamItem *item = s->item();
    RamProject *project = item->project();
    RamStep *step = s->step();
    QString type = "_A_";
    if (item->objectType() == RamObject::Shot) type = "_S_";
    return createPath( path(item) + "/" + project->shortName() + type + item->shortName() + "_" + step->shortName());
}

QDir Ramses::dir(RamStatus *s) const
{
    return QDir( path(s) );
}

void Ramses::setCurrentUser(RamUser *u)
{
    m_currentUser = u;
    if (u)
    {
        m_connected = true;
        emit loggedIn(m_currentUser);
    }
    else
    {
        m_connected = false;
        setCurrentProject(nullptr);
        emit loggedOut();
    }
}

QString Ramses::defaultProjectPath(RamProject *p) const
{
    return projectsPath() + "/" + p->shortName();
}

RamProject *Ramses::currentProject() const
{
    return m_currentProject;
}

void Ramses::init()
{
    if(!m_currentUser) return;
    QSettings *uSettings = m_currentUser->userSettings();
    setCurrentProject(uSettings->value("ramses/currentProject", "").toString());
}

void Ramses::setCurrentProject(RamProject *currentProject)
{
    qDebug() << "Setting project: Database call";
    if ( currentProject ) m_dbi->getProject(currentProject->uuid());
}

void Ramses::projectReady(QString uuid)
{
    RamProject *currentProject = RamProject::project(uuid);
    if ( !currentProject ) return;

    if (currentProject->is( m_currentProject )) return;

    m_currentProject = currentProject;
    if(m_currentUser)
    {
        QSettings *uSettings = m_currentUser->userSettings();
        uSettings->setValue("ramses/currentProject", m_currentProject->uuid() );
    }

    qDebug() << "Setting current project to: " + m_currentProject->shortName();
#ifdef DUMP_OBJECT_DEBUG
    dumpObjectInfo();
#endif
    emit currentProjectChanged(m_currentProject);
}

void Ramses::setCurrentProject(QString uuidOrShortName)
{
    setCurrentProject( RamProject::project(uuidOrShortName) );
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
        m_states->append(m_noState);
    }
    return m_noState;
}

RamState *Ramses::todoState()
{
    if(m_todoState) return m_todoState;
    m_todoState =  qobject_cast<RamState*>( m_states->fromName("TODO") );
    return m_todoState;
}

RamState *Ramses::okState()
{
    if(m_okState) return m_okState;
    m_okState =  qobject_cast<RamState*>( m_states->fromName("OK") );
    return m_okState;
}

void Ramses::logout()
{
    m_connected = false;
    m_currentUser = nullptr;
    emit loggedOut();
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
    // Get current project
    m_dbi->init();
}

bool Ramses::isConnected() const
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

void Ramses::setRamUser(RamUser *user)
{
    m_ramUser = user;
}

RamObjectList *Ramses::users() const
{
    return m_users;
}
