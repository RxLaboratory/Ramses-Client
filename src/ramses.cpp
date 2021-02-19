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
    _dbi = DBInterface::instance();

    DBISuspender s;

    _currentUser = nullptr;
    _currentProject = nullptr;
    _mainPath = _settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString();

    _connected = false;

    connect( _dbi, &DBInterface::data, this, &Ramses::newData );
    connect( _dbi, &DBInterface::connectionStatusChanged, this, &Ramses::dbiConnectionStatusChanged);
    connect( (DuApplication *)qApp, &DuApplication::idle, this, &Ramses::refresh);
}

void Ramses::login(QString username, QString password)
{
    _dbi->login(username, password);
}

void Ramses::newData(QJsonObject data)
{
    if (!data.value("success").toBool()) return;

    QString query = data.value("query").toString();
    if (query == "login") login( data.value("content").toObject() );
    else if (query == "getUsers") gotUsers( data.value("content").toArray());
    else if (query == "getProjects") gotProjects( data.value("content").toArray());
    else if (query == "getTemplateSteps") gotTemplateSteps( data.value("content").toArray());
    else if (query == "getStates") gotStates( data.value("content").toArray());
}

void Ramses::gotUsers(QJsonArray users)
{
    DBISuspender s;

    // loop through existing users to update them
    for (int i = _users.count() - 1; i >= 0; i--)
    {
        RamUser *existingUser = _users[i];
        // loop through new users to update
        bool found = false;
        for (int j = 0; j < users.count(); j++)
        {
            QJsonObject newUser = users[j].toObject();
            QString uuid = newUser.value("uuid").toString();
            // Found, update
            if (uuid == existingUser->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingUser);
                existingUser->setName( newUser.value("name").toString());
                existingUser->setShortName( newUser.value("shortName").toString());
                existingUser->setFolderPath( newUser.value("folderPath").toString());
                //send the signal
                b.unblock();
                QString r = newUser.value("role").toString("standard");
                if (r == "admin") existingUser->setRole(RamUser::Admin);
                else if (r == "lead") existingUser->setRole(RamUser::Lead);
                else existingUser->setRole(RamUser::Standard);
                //remove from new users
                users.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamUser *u = _users.takeAt(i);
            u->deleteLater();
        }
    }

    // loop through remaining new users to add them
    for (int i = 0; i < users.count(); i++)
    {
        QJsonObject u = users[i].toObject();
        RamUser *user = new RamUser(
                    u.value("shortName").toString(),
                    u.value("name").toString(),
                    u.value("uuid").toString()
                    );
        user->setFolderPath(u.value("folderPath").toString());

        QString r = u.value("role").toString("standard");
        if (r == "admin") user->setRole(RamUser::Admin);
        else if (r == "lead") user->setRole(RamUser::Lead);
        else user->setRole(RamUser::Standard);
        _users << user;

        connect(user,&RamUser::destroyed, this, &Ramses::userDestroyed);
        emit newUser(user);
    }

    // Set the current user
    foreach(RamUser *user, _users)
    {
        if (user->shortName() == _currentUserShortName)
        {
            _currentUser = user;
            _connected = true;
            emit loggedIn(_currentUser);

            return;
        }
    }

    // Not found
    _currentUser = nullptr;
    _connected = false;
    emit loggedOut();
}

void Ramses::userDestroyed(QObject *o)
{
    RamUser *u = (RamUser*)o;
    removeUser(u->uuid());
}

void Ramses::gotProjects(QJsonArray projects)
{
    DBISuspender s;

    // loop through existing projects to update them
    for (int i = _projects.count() - 1; i >= 0; i--)
    {
        RamProject *existingProject = _projects[i];
        // loop through new projects to update
        bool found = false;
        for (int j = 0; j < projects.count(); j++)
        {
            QJsonObject newProject = projects[j].toObject();
            QString uuid = newProject.value("uuid").toString();
            // Found, update
            if (uuid == existingProject->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingProject);
                existingProject->setName( newProject.value("name").toString());
                existingProject->setShortName( newProject.value("shortName").toString());
                gotSteps( newProject.value("steps").toArray(), existingProject);
                //send the signal
                b.unblock();
                existingProject->setFolderPath( newProject.value("folderPath").toString());
                //remove from new projects
                projects.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamProject *p = _projects.takeAt(i);
            p->deleteLater();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < projects.count(); i++)
    {
        QJsonObject p = projects[i].toObject();
        RamProject *project = new RamProject(
                    p.value("shortName").toString(),
                    p.value("name").toString(),
                    p.value("uuid").toString()
                    );
        project->setFolderPath( p.value("folderPath").toString());

        // Add steps
        gotSteps( p.value("steps").toArray(), project);

        _projects << project;

        connect(project,&RamProject::destroyed, this, &Ramses::projectDestroyed);

        emit newProject(project);
    }

}

void Ramses::projectDestroyed(QObject *o)
{
    RamProject *p = (RamProject*)o;
    removeProject(p->uuid());
}

void Ramses::gotTemplateSteps(QJsonArray steps)
{
    DBISuspender s;

    // loop through existing steps to update them
    for (int i = _templateSteps.count() - 1; i >= 0; i--)
    {
        RamStep *existingStep = _templateSteps[i];
        // loop through new steps to update
        bool found = false;
        for (int j = 0; j < steps.count(); j++)
        {
            QJsonObject newStep = steps[j].toObject();
            QString uuid = newStep.value("uuid").toString();
            // Found, update
            if (uuid == existingStep->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingStep);
                existingStep->setName( newStep.value("name").toString());
                existingStep->setShortName( newStep.value("shortName").toString());
                //send the signal
                b.unblock();
                existingStep->setType(newStep.value("type").toString());
                //remove from new projects
                steps.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamStep *s = _templateSteps.takeAt(i);
            s->deleteLater();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < steps.count(); i++)
    {
        QJsonObject s = steps[i].toObject();
        RamStep *step = new RamStep(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    true,
                    s.value("uuid").toString()
                    );
        step->setType( s.value("type").toString());

        _templateSteps << step;

        connect(step,&RamStep::destroyed, this, &Ramses::templateStepDestroyed);

        emit newTemplateStep(step);
    }
}

void Ramses::templateStepDestroyed(QObject *o)
{
    RamStep *s = (RamStep*)o;
    removeTemplateStep(s->uuid());
}

void Ramses::gotStates(QJsonArray states)
{
    DBISuspender s;

    // loop through existing steps to update them
    for (int i = _states.count() - 1; i >= 0; i--)
    {
        RamState *existingState = _states[i];
        // loop through new steps to update
        bool found = false;
        for (int j = 0; j < states.count(); j++)
        {
            QJsonObject newState = states[j].toObject();
            QString uuid = newState.value("uuid").toString();
            // Found, update
            if (uuid == existingState->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingState);
                existingState->setName( newState.value("name").toString());
                existingState->setShortName( newState.value("shortName").toString());
                existingState->setColor( QColor(newState.value("color").toString()));
                //send the signal
                b.unblock();
                existingState->setCompletionRatio(newState.value("completionRatio").toInt());
                //remove from new projects
                states.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            RamState *s = _states.takeAt(i);
            s->deleteLater();
        }
    }

    // loop through remaining new projects to add them
    for (int i = 0; i < states.count(); i++)
    {
        QJsonObject s = states[i].toObject();
        RamState *state = new RamState(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    s.value("uuid").toString()
                    );
        state->setColor( QColor( s.value("color").toString()) );
        state->setCompletionRatio( s.value("completionRatio").toInt() );

        _states << state;

        connect(state,&RamState::destroyed, this, &Ramses::stateDestroyed);

        emit newState(state);
    }
}

void Ramses::stateDestroyed(QObject *o)
{
    RamState *s = (RamState*)o;
    removeState(s->uuid());
}

void Ramses::gotSteps(QJsonArray steps, RamProject *project)
{
    DBISuspender s;

    QList<RamStep*> projectSteps = project->steps();

    // loop through existing steps to update them
    for (int i = projectSteps.count() - 1; i >= 0; i--)
    {
        RamStep *existingStep = projectSteps[i];
        // loop through new steps to update
        bool found = false;
        for (int j = 0; j < steps.count(); j++)
        {
            QJsonObject newStep = steps[j].toObject();
            QString uuid = newStep.value("uuid").toString();
            // Found, update
            if (uuid == existingStep->uuid())
            {
                found = true;
                //Emit just one signal
                QSignalBlocker b(existingStep);
                existingStep->setName( newStep.value("name").toString());
                existingStep->setShortName( newStep.value("shortName").toString());
                //send the signal
                b.unblock();
                existingStep->setType(newStep.value("type").toString());
                //remove from new steps
                steps.removeAt(j);
                break;
            }
        }
        // Not found, remove from existing
        if (!found)
        {
            project->removeStep(existingStep);
        }
    }

    // loop through remaining new steps to add them
    for (int i = 0; i < steps.count(); i++)
    {
        QJsonObject s = steps[i].toObject();
        RamStep *step = new RamStep(
                    s.value("shortName").toString(),
                    s.value("name").toString(),
                    false,
                    s.value("uuid").toString()
                    );
        step->setType( s.value("type").toString());
        step->setProjectUuid( s.value("projectUuid").toString());

        project->addStep(step);

        emit newStep(step);
    }
}

void Ramses::dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s)
{
    if (s != NetworkUtils::Online)
    {
        QSignalBlocker b(_dbi);
        logout();
    }
}

void Ramses::login(QJsonObject user)
{
    // Set the current user shortName
    _currentUserShortName = user.value("shortName").toString("Guest");
    // Update
    refresh();
}

QString Ramses::mainPath() const
{
    return _mainPath;
}

void Ramses::setMainPath(const QString &mainPath)
{
    _mainPath = mainPath;
}

QString Ramses::pathFromMain(QString p) const
{
    if (QFileInfo( p ).isRelative())
    {
        return mainPath() + "/" +p;
    }
    else
    {
        return p;
    }
}

QString Ramses::usersPath() const
{
    return mainPath() + "/Users";
}

QString Ramses::userPath(RamUser *u) const
{
    if (!u) return pathFromMain("");

    QString path = u->folderPath();
    if (path == "" || path == "auto") path = usersPath() + "/" + u->shortName();
    return pathFromMain( path );
}

QString Ramses::defaultUserPath(RamUser *u) const
{
    return usersPath() + "/" + u->shortName();
}

QString Ramses::projectsPath() const
{
    return mainPath() + "/Projects";
}

QString Ramses::projectPath(RamProject *p) const
{
    if (!p) return pathFromMain("");

    QString path = p->folderPath();
    if (path == "" || path == "auto") path = projectsPath() + "/" + p->shortName();
    return pathFromMain( path );
}

QString Ramses::defaultProjectPath(RamProject *p)
{
    return projectsPath() + "/" + p->shortName();
}

RamProject *Ramses::currentProject() const
{
    return _currentProject;
}

void Ramses::setCurrentProject(RamProject *currentProject)
{
    _currentProject = currentProject;
    emit projectChanged(_currentProject);
}

void Ramses::setCurrentProject(QString uuid)
{
    setCurrentProject( project(uuid) );
}

QList<RamStep *> Ramses::templateSteps() const
{
    return _templateSteps;
}

RamStep *Ramses::createTemplateStep()
{
    RamStep *step = new RamStep("New", "Step", true);
    step->setParent(this);
    _templateSteps << step;
    emit newTemplateStep(step);
    return step;
}

void Ramses::removeTemplateStep(QString uuid)
{
    for (int i = _templateSteps.count() -1; i >= 0; i--)
    {
        if (_templateSteps[i]->uuid() == uuid)
        {
            RamStep *s = _templateSteps.takeAt(i);
            s->deleteLater();
        }
    }
}

RamStep *Ramses::templateStep(QString uuid)
{
    foreach(RamStep *step, _templateSteps)
    {
        if (step->uuid() == uuid) return step;
    }
    return nullptr;
}

QList<RamState *> Ramses::states() const
{
    return _states;
}

RamState *Ramses::createState()
{
    RamState *state = new RamState("New", "State");
    state->setParent(this);
    _states << state;
    emit newState(state);
    return state;
}

void Ramses::removeState(QString uuid)
{
    for (int i = _states.count() -1; i >= 0; i--)
    {
        if (_states[i]->uuid() == uuid)
        {
            RamState *s = _states.takeAt(i);
            s->deleteLater();
        }
    }
}

QList<RamProject *> Ramses::projects() const
{
    return _projects;
}

RamProject *Ramses::project(QString uuid)
{
    foreach(RamProject *p, _projects)
    {
        if (p->uuid() == uuid) return p;
    }
    return nullptr;
}

RamProject *Ramses::createProject()
{
    RamProject *project = new RamProject("New","Project");
    project->setParent(this);
    _projects << project;
    emit newProject(project);
    return project;
}

void Ramses::removeProject(QString uuid)
{
    for (int i = _projects.count() -1; i >= 0; i--)
    {
        if (_projects[i]->uuid() == uuid)
        {
            RamProject *p = _projects.takeAt(i);
            p->deleteLater();
        }
    }
}

RamUser *Ramses::createUser()
{
    RamUser *user = new RamUser("New","J. Doe");
    user->setParent(this);
    _users << user;
    emit newUser(user);
    return user;
}

void Ramses::removeUser(QString uuid)
{
    for (int i = _users.count() -1; i >= 0; i--)
    {
        if (_users[i]->uuid() == uuid)
        {
            RamUser *u = _users.takeAt(i);
            u->deleteLater();
        }
    }
}

bool Ramses::isAdmin()
{
    if (!_currentUser) return false;
    return _currentUser->role() == RamUser::Admin;
}

void Ramses::logout()
{
    _connected = false;
    _currentUser = nullptr;
    emit loggedOut();
}

void Ramses::refresh()
{
    // Get Users
    _dbi->getUsers();
    // Get Template Steps
    _dbi->getTemplateSteps();
    // Get States
    _dbi->getStates();
    // Get Projects
    _dbi->getProjects();
}

bool Ramses::isConnected() const
{
    return _connected;
}

RamUser *Ramses::currentUser() const
{
    return _currentUser;
}

QList<RamUser *> Ramses::users() const
{
    return _users;
}
