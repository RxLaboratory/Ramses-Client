#include "daemon.h"

Daemon *Daemon::_instance = nullptr;

Daemon* Daemon::instance()
{
    if (!_instance) _instance = new Daemon();
    return _instance;
}

void Daemon::start()
{
    if (!_tcpServer->listen( QHostAddress::LocalHost, _settings.value("daemon/port", 18185).toInt() )) {
        qDebug() << _tcpServer->errorString();
        log("Unable to start the daemon server.\n" + _tcpServer->errorString(), DuQFLog::Warning);
    }
    else
    {
        qDebug() << "Daemon started and listening on port " + QString::number(_tcpServer->serverPort());
        log("Daemon started and listening on port " + QString::number(_tcpServer->serverPort()), DuQFLog::Information);
    }
}

void Daemon::stop()
{
    _tcpServer->close();
    log("Daemon stopped.", DuQFLog::Information);
}

void Daemon::restart()
{
    stop();
    start();
}

void Daemon::newConnection()
{
    QTcpSocket *client = _tcpServer->nextPendingConnection();
    connect(client, &QAbstractSocket::disconnected, client, &QObject::deleteLater);
    connect(client, &QAbstractSocket::readyRead, this, &Daemon::reply);
}

void Daemon::reply()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString request = client->readAll();
    //split args
    QStringList requestArgs = request.split("&");
    log("I've got these args: \n" + requestArgs.join("\n"), DuQFLog::Debug);

    //Parse
    QMap<QString, QString> args;
    for(const QString &arg: qAsConst(requestArgs))
    {
        QStringList aList = arg.split("=");
        if (aList.count() > 1)
        {
            args[aList.at(0)] = aList.at(1);
        }
        else
        {
            args[aList.at(0)] = "";
        }
    }

    //Read
    if (args.contains("ping"))
        ping(client);
    else if (args.contains("raise"))
        emit raise();
    else if (args.contains("setCurrentProject"))
        setCurrentProject(args.value("shortName"), client);
    else if (args.contains("getAssets"))
        getAssets(client);
    else if (args.contains("getProjects"))
        getProjects(client);
    else if (args.contains("getShots"))
        getShots(client);
    else
        post(client, QJsonObject(), "", "Unknown query.", false, false);

}

void Daemon::ping(QTcpSocket *client)
{
    log("I'm replying to this request: ping", DuQFLog::Information);

    QJsonObject content;
    content.insert("version", STR_VERSION);
    content.insert("ramses", STR_INTERNALNAME);
    RamUser *user = Ramses::instance()->currentUser();
    if (user)
    {
        content.insert("userName", user->name());
        content.insert("userShortName", user->name());
        content.insert("logged-in", true);
    }
    else
    {
        content.insert("userName", "");
        content.insert("userShortName", "");
        content.insert("logged-in", false);
    }
    post(client, content, "ping","Hi, this is the Ramses Daemon");
}

void Daemon::setCurrentProject(QString shortName, QTcpSocket *client)
{
    log("I'm replying to this request: setCurrentProject: " + shortName, DuQFLog::Information);

    Ramses::instance()->setCurrentProject(shortName);

    QJsonObject content;
    RamProject *p = Ramses::instance()->currentProject();
    if (p)
    {
        content.insert("name", p->name());
        content.insert("shortName", p->shortName());
        content.insert("path", p->folderPath());
        content.insert("uuid", p->uuid());
        post(client, content, "setCurrentProject", "Current project set to: " + p->name());
    }
    else
    {
        content.insert("name", "");
        content.insert("shortName", "");
        content.insert("path", "");
        content.insert("uuid", "");
        post(client, content, "setCurrentProject", "Project " + shortName + " not found, sorry!", false);
    }
}

void Daemon::getAssets(QTcpSocket *client)
{
    log("I'm replying to this request: getAssets", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getAssets", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray assets;
    foreach(RamAsset *a, proj->assets())
    {
        QJsonObject asset;
        asset.insert("shortName", a->shortName());
        asset.insert("name", a->name());
        asset.insert("folder", Ramses::instance()->path(a));
        QJsonArray tags;
        foreach(QString t, a->tags())
        {
            tags.append(t);
        }
        asset.insert("tags", tags);
        RamAssetGroup *ag = proj->assetGroup( a->assetGroupUuid() );
        if (ag) asset.insert("group", ag->name());
        else asset.insert("group", "");
        assets.append(asset);
    }
    content.insert("assets", assets);
    post(client, content, "getASsets", "Asset list retrieved.");
}

void Daemon::getProjects(QTcpSocket *client)
{
    log("I'm replying to this request: getProjects", DuQFLog::Information);

    QJsonObject content;
    QJsonArray projects;
    foreach(RamProject *p, Ramses::instance()->projects())
    {
        QJsonObject project;
        project.insert("shortName", p->shortName());
        project.insert("name", p->name());
        project.insert("folder", Ramses::instance()->path(p));
        projects.append(project);
    }
    content.insert("projects", projects);

    post(client, content, "getProjects", "Project list retrived");
}

void Daemon::getShots(QTcpSocket *client)
{
    log("I'm replying to this request: getShots", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getShots", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray shots;
    foreach(RamShot *s, proj->shots())
    {
        QJsonObject shot;
        shot.insert("shortName", s->shortName());
        shot.insert("name", s->name());
        shot.insert("folder", Ramses::instance()->path(s));
        shot.insert("duration", s->duration());
        shots.append(shot);
    }
    content.insert("shots", shots);
    post(client, content, "getShots", "Shots list retrieved.");
}

Daemon::Daemon(QObject *parent) : DuQFLoggerObject("Daemon", parent)
{
    _tcpServer = new QTcpServer(this);

    start();

    connect(_tcpServer, &QTcpServer::newConnection, this, &Daemon::newConnection);
}

void Daemon::post(QTcpSocket *client, QJsonObject content, QString query, QString message, bool success, bool accepted)
{
    QJsonObject obj;
    obj.insert("query", query);
    obj.insert("message", message);
    obj.insert("accepted", accepted);
    obj.insert("success", success);
    obj.insert("content", content);
    QJsonDocument json(obj);

    QString jsonReply = json.toJson();
    client->write( jsonReply.toUtf8() );

    log("Posting:\n" + jsonReply, DuQFLog::Debug);
}
