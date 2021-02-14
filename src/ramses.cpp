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

    _defaultUser = new RamUser("Guest", "J. Doe", "", "", this);
    _currentUser = _defaultUser;
    _users << _defaultUser;

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
}

void Ramses::gotUsers(QJsonArray users)
{
    while( _users.count() > 1)
    {
        RamUser *u = _users.takeLast();
        u->deleteLater();
    }
    for (int i = 0; i < users.count(); i++)
    {
        QJsonObject u = users[i].toObject();

        qDebug() << u;

        RamUser *user = new RamUser(
                    u.value("shortName").toString(),
                    u.value("name").toString(),
                    u.value("uuid").toString(),
                    u.value("folderPath").toString(),
                    this);

        QString r = u.value("role").toString("standard");
        if (r == "admin") user->setRole(RamUser::Admin);
        else if (r == "lead") user->setRole(RamUser::Lead);

        qDebug() << "Got user: " + user->shortName();

        _users << user;
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

void Ramses::logout()
{
    _connected = false;
    _currentUser = _defaultUser;
    _dbi->setOffline();
    emit loggedOut();
}

void Ramses::update()
{
    // Get Users
    _dbi->getUsers();
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
