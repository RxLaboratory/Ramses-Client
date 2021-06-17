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
    _dbi = DBInterface::instance();

    _users = new RamObjectList(this);
    _states = new RamStateList(this);
    _projects = new RamObjectList(this);
    _templateSteps = new RamObjectList(this);
    _templateAssetGroups = new RamObjectList(this);
    _fileTypes = new RamObjectList (this);
    _applications = new RamObjectList(this);
    _ramUser = nullptr;

    DBISuspender s;

    _currentUser = nullptr;
    _currentProject = nullptr;
    _ramsesPath = _settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString();

    _connected = false;

    _userSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, STR_COMPANYNAME, STR_INTERNALNAME);
    _userSettings->beginGroup("ramses");

    connect( (DuApplication *)qApp, &DuApplication::idle, this, &Ramses::refresh);
    connect( _dbi, &DBInterface::connectionStatusChanged, this, &Ramses::dbiConnectionStatusChanged);

    this->setObjectName( "Ramses Class" );

    qDebug() << "Ramses Ready!";
}

void Ramses::login(QString username, QString password)
{
    _dbi->login(username, password);
}

void Ramses::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s != NetworkUtils::Online)
    {
        QSignalBlocker b(_dbi);
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
    return createPath( _ramsesPath );
}

void Ramses::setRamsesPath(const QString &ramsesPath)
{
    _ramsesPath = ramsesPath;
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
    RamProject *p = project( s->projectUuid() );
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

QString Ramses::path(RamAsset *a) const
{
    RamAssetGroup *ag = a->assetGroup();
    if (!ag) return "";
    RamProject *p = ag->project();
    if (!p) return "";
    return createPath( path(ag) + "/" + p->shortName() + "_A_" + a->shortName() );
}

QDir Ramses::dir(RamAsset *a) const
{
    return QDir(path(a));
}

QString Ramses::path(RamShot *s) const
{
    RamSequence *seq = s->sequence();
    if (!seq) return "";
    RamProject *p = seq->project();
    if (!p) return "";
    return createPath( shotsPath(p) + "/" + p->shortName() + "_S_" + s->name() );
}

QDir Ramses::dir(RamShot *s) const
{
    return QDir(path(s));
}

void Ramses::setCurrentUser(RamUser *u)
{
    _currentUser = u;
    if (u)
    {
        _connected = true;

        // Set settings
        _userSettings->endGroup();
        delete _userSettings;
        _userSettings = new QSettings(Ramses::instance()->currentUserSettingsFile(), QSettings::IniFormat, this);
        _userSettings->beginGroup("ramses");

        emit loggedIn(_currentUser);
    }
    else
    {
        _connected = false;
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
    return _currentProject;
}

void Ramses::init()
{
    setCurrentProject(_userSettings->value("currentProject", "").toString());
}

void Ramses::setCurrentProject(RamProject *currentProject)
{
    if ( currentProject ) _dbi->getProject(currentProject->uuid());
}

void Ramses::projectReady(QString uuid)
{
    RamProject *currentProject = project(uuid);
    if ( !currentProject ) return;

    if (currentProject->is( _currentProject )) return;

    _currentProject = currentProject;
    _userSettings->setValue("currentProject", _currentProject->uuid() );

    emit currentProjectChanged(_currentProject);
}

void Ramses::setCurrentProject(QString uuidOrShortName)
{
    setCurrentProject( project(uuidOrShortName) );
}

RamObjectList *Ramses::templateSteps() const
{
    return _templateSteps;
}

RamStep *Ramses::createTemplateStep()
{
    RamStep *step = new RamStep("NEW", "Step", true);
    step->setParent(this);
    _templateSteps->append(step);
    return step;
}

RamStep *Ramses::templateStep(QString uuid)
{
    return (RamStep*)_templateSteps->fromUuid( uuid );
}

RamObjectList *Ramses::templateAssetGroups() const
{
    return _templateAssetGroups;
}

RamAssetGroup *Ramses::createTemplateAssetGroup()
{
    RamAssetGroup *ag = new RamAssetGroup("NEW", "Asset Group", "", this);
    ag->setParent(this);
    _templateAssetGroups->append(ag);
    return ag;
}

RamAssetGroup *Ramses::templateAssetGroup(QString uuid)
{
    return qobject_cast<RamAssetGroup*>( _templateAssetGroups->fromUuid(uuid) );
}

RamAssetGroup *Ramses::assetGroup(QString uuid) const
{
    for (int i =0; i < _projects->count(); i++)
    {
        RamProject *p = qobject_cast<RamProject*>( _projects->at(i) );
        RamAssetGroup *ag  = qobject_cast<RamAssetGroup*>( p->assetGroups()->fromUuid(uuid) );
        if (ag) return ag;
    }
    return nullptr;
}

RamSequence *Ramses::sequence(QString uuid) const
{
    for (int i =0; i < _projects->count(); i++)
    {
        RamProject *p = (RamProject*)_projects->at(i);
        RamSequence *s = qobject_cast<RamSequence*>( p->sequences()->fromUuid(uuid) );
        if (s) return s;
    }
    return nullptr;
}

RamStateList *Ramses::states() const
{
    return _states;
}

RamState *Ramses::createState()
{
    RamState *state = new RamState("NEW", "State");
    state->setParent(this);
    _states->append(state);
    return state;
}

RamObjectList *Ramses::fileTypes() const
{
    return _fileTypes;
}

RamFileType *Ramses::createFileType()
{
    RamFileType *ft = new RamFileType("NEW", "New File Type", QStringList("new"));
    ft->setParent(this);
    _fileTypes->append(ft);
    return ft;
}

RamObjectList *Ramses::applications() const
{
    return _applications;
}

RamApplication *Ramses::createApplication()
{
    RamApplication *a = new RamApplication("New", "New Application");
    a->setParent(this);
    _applications->append(a);
    return a;
}

RamObjectList *Ramses::projects() const
{
    return _projects;
}

RamProject *Ramses::project(QString uuid) const
{
    return (RamProject*)_projects->fromUuid( uuid );
}

RamProject *Ramses::createProject()
{
    RamProject *project = new RamProject("NEW","Project");
    project->setParent(this);
    _projects->append( project );
    return project;
}

RamUser *Ramses::createUser()
{
    RamUser *user = new RamUser("NEW","J. Doe");
    user->setParent(this);
    _users->append(user);
    return user;
}

bool Ramses::isAdmin()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::Admin;
}

bool Ramses::isProjectAdmin()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::ProjectAdmin;
}

bool Ramses::isLead()
{
    if (!_currentUser) return false;
    return _currentUser->role() >= RamUser::Lead;
}

QString Ramses::currentUserSettingsFile()
{
    QString settingsPath = currentUserConfigPath();
    if (settingsPath == "") return "";
    return settingsPath + "/" + "ramses.ini";
}

RamState *Ramses::noState()
{
    if(_noState) return _noState;
    _noState =  qobject_cast<RamState*>( _states->fromName("NO") );
    return _noState;
}

RamState *Ramses::todoState()
{
    if(_todoState) return _todoState;
    _todoState =  qobject_cast<RamState*>( _states->fromName("TODO") );
    return _todoState;
}

RamState *Ramses::okState()
{
    if(_okState) return _okState;
    _okState =  qobject_cast<RamState*>( _states->fromName("OK") );
    return _okState;
}

void Ramses::logout()
{
    _connected = false;
    _currentUser = nullptr;
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
    _dbi->init();
}

bool Ramses::isConnected() const
{
    return _connected;
}

RamUser *Ramses::currentUser() const
{
    return _currentUser;
}

RamUser *Ramses::ramUser()
{
    if (_ramUser) return _ramUser;
    _ramUser = qobject_cast<RamUser*>( _users->fromName("Ramses") );
    return _ramUser;
}

void Ramses::setRamUser(RamUser *user)
{
    _ramUser = user;
}

RamObjectList *Ramses::users() const
{
    return _users;
}
