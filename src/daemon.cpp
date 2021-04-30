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
    else if (args.contains("getCurrentProject"))
        getCurrentProject(client);
    else if (args.contains("getCurrentUser"))
        getCurrentUser(client);
    else if (args.contains("getPipes"))
        getPipes(client);
    else if (args.contains("getProjects"))
        getProjects(client);
    else if (args.contains("getShots"))
        getShots(args.value("filter", ""), client);
    else if (args.contains("getStates"))
        getStates(client);
    else if (args.contains("getSteps"))
        getSteps(client);
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

void Daemon::getCurrentProject(QTcpSocket *client)
{
    log("I'm replying to this request: getCurrentProject", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getCurrentProject", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    content.insert("shortName", proj->shortName());
    content.insert("name", proj->name());
    content.insert("width", proj->width());
    content.insert("height", proj->height());
    content.insert("framerate", proj->framerate());
    content.insert("folder", Ramses::instance()->path(proj));

    post(client, content, "getCurrentProject", "Current project is: " + proj->name());
}

void Daemon::getCurrentUser(QTcpSocket *client)
{
    log("I'm replying to this request: getCurrentUser", DuQFLog::Information);

    RamUser *user = Ramses::instance()->currentUser();

    QJsonObject content;

    if (!user)
    {
        post(client, content, "getCurrentUser", "Sorry, there's no current user. You need to log in first!", false);
        return;
    }

    content.insert("shortName", user->shortName());
    content.insert("name", user->name());
    content.insert("folderPath", Ramses::instance()->path(user));
    RamUser::UserRole role = user->role();
    if (role == RamUser::Admin) content.insert("role", "ADMIN");
    else if (role == RamUser::ProjectAdmin) content.insert("role","PROJECT_ADMIN");
    else if (role == RamUser::Lead) content.insert("role", "LEAD");
    else content.insert("role", "STANDARD");

    post( client, content, "getCurrentUser", "Current user is: " + user->name());
}

void Daemon::getPipes(QTcpSocket *client)
{
    log("I'm replying to this request: getPipes", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getPipes", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray pipes;
    foreach(RamPipe *p, proj->pipeline())
    {
        QJsonObject pipe;
        if (!p->inputStep()) continue;
        if (!p->outputStep()) continue;
        if (!p->fileType()) continue;
        pipe.insert("inputStepShortName", p->inputStep()->shortName());
        pipe.insert("outputStepShortName", p->outputStep()->shortName());
        QJsonObject fileType;
        fileType.insert("name", p->fileType()->name());
        fileType.insert("shortName", p->fileType()->shortName());
        QJsonArray extensions = QJsonArray::fromStringList( p->fileType()->extensions() );
        fileType.insert("extensions", extensions);
        pipe.insert("fileType", fileType);
        pipes.append(pipe);
    }
    content.insert("pipes", pipes);
    post(client, content, "getPipes", "Pipeline retrieved.");
}

void Daemon::getProjects(QTcpSocket *client)
{
    log("I'm replying to this request: getProjects", DuQFLog::Information);

    QJsonObject content;
    QJsonArray projects;
    for (int i = 0; i < Ramses::instance()->projects()->count(); i++)
    {
        RamProject *p = (RamProject*)Ramses::instance()->projects()->at(i);
        QJsonObject project;
        project.insert("shortName", p->shortName());
        project.insert("name", p->name());
        project.insert("width", p->width());
        project.insert("height", p->height());
        project.insert("framerate", p->framerate());
        project.insert("folder", Ramses::instance()->path(p));
        projects.append(project);
    }
    content.insert("projects", projects);

    post(client, content, "getProjects", "Project list retrived");
}

void Daemon::getShots(QString filter, QTcpSocket *client)
{
    log("I'm replying to this request: getShots", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getShots", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    if (filter == "") filter = "*";
    filter = QRegularExpression::escape(filter);
    filter = filter.replace("\\*", "([a-z0-9+-]*\\s*)*");
    log("Using the regexp:" + filter, DuQFLog::Debug);
    QRegularExpression re(filter,QRegularExpression::CaseInsensitiveOption);

    QJsonArray shots;
    for (int i = 0; i < proj->sequences()->objectCount(); i++)
    {
        RamShot *s = (RamShot*)proj->sequences()->objectAt(i);

        bool ok = filter == "([a-z0-9+-]*\\s*)*";
        if (!ok)
        {
            ok = re.match( s->shortName() ).hasMatch();
            if (!ok) ok = re.match( s->name() ).hasMatch();
        }
        if (!ok) continue;

        QJsonObject shot;
        shot.insert("shortName", s->shortName());
        shot.insert("name", s->name());
        shot.insert("folder", Ramses::instance()->path(s));
        shot.insert("duration", s->duration());
        shots.append(shot);
    }
    content.insert("shots", shots);
    post(client, content, "getShots", "Shot list retrieved.");
}

void Daemon::getStates(QTcpSocket *client)
{
    log("I'm replying to this request: getStates", DuQFLog::Information);

    QJsonObject content;
    QJsonArray states;
    RamStateList *ramStates = Ramses::instance()->states();
    for (int i = 0; i < ramStates->count(); i++)
    {
        RamState *s = (RamState*)ramStates->at(i);
        QJsonObject state;
        state.insert("shortName", s->shortName());
        state.insert("name", s->name());
        state.insert("completionRatio", s->completionRatio());
        QColor col = s->color();
        QJsonArray cols;
        cols.append( col.red() );
        cols.append( col.green() );
        cols.append( col.blue() );
        state.insert("color", cols);
        states.append(state);
    }
    content.insert("states", states);

    post(client, content, "getStates", "State list retrived");
}

void Daemon::getSteps(QTcpSocket *client)
{
    log("I'm replying to this request: getSteps", DuQFLog::Information);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getSteps", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray steps;
    foreach(RamStep *s, proj->steps())
    {
        QJsonObject step;
        step.insert("shortName", s->shortName());
        step.insert("name", s->name());
        step.insert("folder", Ramses::instance()->path(s));
        QString type;
        switch (s->type())
        {
        case RamStep::PreProduction:
            type = "PRE_PRODUCTION";
            break;
        case RamStep::AssetProduction:
            type = "ASSET_PRODUCTION";
            break;
        case RamStep::ShotProduction:
            type = "SHOT_PRODUCTION";
            break;
        case RamStep::PostProduction:
            type ="POST_PRODUCTION";
            break;
        default:
            type = "SHOT_PRODUCTION";
        }
        step.insert("type", type);
        steps.append(step);
    }
    content.insert("steps", steps);
    post(client, content, "getSteps", "Step list retrieved.");
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

    log("Posting:\n" + jsonReply, DuQFLog::Information);
}
