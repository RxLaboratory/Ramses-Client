#include "ramses.h"

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::instance()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

Ramses::Ramses(QObject *parent) : QObject(parent)
{
    _dbi = DBInterface::instance();

    DBISuspender s;
    _defaultUser = new RamUser("Guest", "J. Doe", "", "", this);
    _currentUser = _defaultUser;

    _connected = false;

    connect( _dbi, &DBInterface::data, this, &Ramses::newData );
    connect( _dbi, &DBInterface::connectionStatusChanged, this, &Ramses::dbiConnectionStatusChanged);
    connect( (DuApplication *)qApp, &DuApplication::idle, this, &Ramses::update);
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
                    u.value("uuid").toString(),
                    u.value("folderPath").toString()
                    );

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
    _currentUser = _defaultUser;
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
                    p.value("uuid").toString(),
                    p.value("folderPath").toString()
                    );

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
    update();
}

QList<RamProject *> Ramses::projects() const
{
    return _projects;
}

RamProject *Ramses::createProject()
{
    RamProject *project = new RamProject("New","Project");
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

RamUser *Ramses::defaultUser() const
{
    return _defaultUser;
}

RamUser *Ramses::createUser()
{
    RamUser *user = new RamUser("New","J. Doe");
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
    _currentUser = _defaultUser;
    emit loggedOut();
}

void Ramses::update()
{
    // Get Users
    _dbi->getUsers();
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
