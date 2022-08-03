#include "daemon.h"

Daemon *Daemon::_instance = nullptr;

Daemon* Daemon::instance()
{
    if (!_instance) _instance = new Daemon();
    return _instance;
}

void Daemon::start()
{
    if (!m_tcpServer->listen( QHostAddress::LocalHost, m_settings.value("daemon/port", 18185).toInt() )) {
        qDebug() << m_tcpServer->errorString();
        log("Unable to start the daemon server.\n" + m_tcpServer->errorString(), DuQFLog::Warning);
    }
    else
    {
        qDebug() << "Daemon started and listening on port " + QString::number(m_tcpServer->serverPort());
        log("Daemon started and listening on port " + QString::number(m_tcpServer->serverPort()), DuQFLog::Information);
    }
}

void Daemon::stop()
{
    m_tcpServer->close();
    log("Daemon stopped.", DuQFLog::Information);
}

void Daemon::restart()
{
    stop();
    start();
}

void Daemon::suspend()
{
    qDebug() << "Suspending Daemon";
    m_suspended = true;
}

void Daemon::resume()
{
    qDebug() << "Resuming Daemon";
    m_suspended = false;
    while ( !m_queue.isEmpty() ) reply(m_queue.takeFirst(), m_waitingClients.takeFirst());
}

void Daemon::newConnection()
{
    QTcpSocket *client = m_tcpServer->nextPendingConnection();
    connect(client, &QAbstractSocket::disconnected, client, &QObject::deleteLater);
    connect(client, SIGNAL(readyRead()), this,SLOT(reply()));
}

void Daemon::reply()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString request = client->readAll();
    if (m_suspended)
    {
        m_queue << request;
        m_waitingClients << client;
        return;
    }

    reply(request, client);
}

void Daemon::reply(QString request, QTcpSocket *client)
{
    //split args
    QStringList requestArgs = request.split("&");
    log("I've got these args: \n" + requestArgs.join("\n"), DuQFLog::Debug);

    //Parse
    QMap<QString, QString> args;
    for(const QString &arg: qAsConst(requestArgs))
    {
        QStringList aList = arg.split("=");
        QString key = aList.at(0);
        if (key == "") continue;
        if (aList.count() > 1)
        {
            args[key] = aList.at(1);
        }
        else
        {
            args[key] = "";
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
    else if (args.contains("getAsset"))
        getAsset(args.value("shortName"), args.value("name", ""), client);
    else if (args.contains("getAssetGroups"))
        getAssetGroups(client);
    else if (args.contains("getSequences"))
        getSequences(client);
    else if (args.contains("getCurrentProject"))
        getProject("","",client);
    else if (args.contains("getProject"))
        getProject(args.value("shortName"), args.value("name", ""),client);
    else if (args.contains("getCurrentStatuses"))
        getCurrentStatus(args.value("shortName"), args.value("name", ""), args.value("type"),"", client);
    else if (args.contains("getCurrentStatus"))
        getCurrentStatus(
                    args.value("shortName"),
                    args.value("name", ""),
                    args.value("type"),
                    args.value("step"),
                    client
                    );
    else if (args.contains("setStatus"))
        setStatus(
                    args.value("shortName"),
                    args.value("name", ""),
                    args.value("step"),
                    args.value("type"),
                    args.value("state"),
                    args.value("comment"),
                    args.value("completionRatio").toInt(),
                    args.value("version").toInt(),
                    args.value("published") == "1",
                    args.value("user"),
                    client
                    );
    else if (args.contains("getCurrentUser"))
        getCurrentUser(client);
    else if (args.contains("getPipes"))
        getPipes(client);
    else if (args.contains("getProjects"))
        getProjects(client);
    else if (args.contains("getShots"))
        getShots(args.value("filter", ""), client);
    else if (args.contains("getShot"))
        getShot(args.value("shortName"), args.value("name", ""), client);
    else if (args.contains("getStates"))
        getStates(client);
    else if (args.contains("getState"))
        getState(args.value("shortName"), args.value("name", ""), client);
    else if (args.contains("getSteps"))
        getSteps(client);
    else if (args.contains("getStep"))
        getStep(args.value("shortName"), args.value("name", ""), client);
    else if (args.contains("getRamsesFolderPath"))
        getRamsesFolder(client);
    else if (args.contains("setPublishSettings"))
        setPublishSettings(
                    args.value("shortName"),
                    args.value("name", ""),
                    args.value("settings", ""),
                    client
                    );
    else
        post(client, QJsonObject(), "", "Unknown query: " + request, false, false);

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
        content.insert("path", p->path(RamObject::NoFolder, true));
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

void Daemon::getCurrentStatus(QString shortName, QString name, QString type, QString stepName, QTcpSocket *client)
{
    log("I'm replying to this request: getCurrentStatus", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getCurrentStatus", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray statuses;

    RamItem *item = nullptr;
    if (type == "A") item = qobject_cast<RamItem*>( proj->assets()->fromName(shortName, name) );
    else item = qobject_cast<RamItem*>( proj->shots()->fromName(shortName, name) );

    if (!item)
    {
        post(client, content, "getCurrentStatus", "Sorry, item not found. Check its name, short name and type", false);
        return;
    }

    // Get step from name
    if ( stepName != "" )
    {
        RamObject *stepObj = proj->steps()->fromName( stepName );
        RamStepStatusHistory *st = item->statusHistory(stepObj);
        if (!st)
        {
            post(client, content, "getCurrentStatus", "Sorry, step " + stepName + " not found.", false);
            return;
        }

        RamStatus *s = item->status( st->step() );
        if (s)
        {
            content = statusToJson(s);
            post(client, content, "getCurrentStatus", "Current status for " + item->name() + " retrieved.");
            return;
        }

        post(client, content, "getCurrentStatus", "Current status for " + item->name() + " not found, sorry.", false);
        return;
    }

    QList<RamStatus*> stss = item->status();
    for(int i = 0; i < statuses.count(); i++)
        statuses.append( statusToJson( stss.at(i) ) );

    content.insert("status", statuses);
    post(client, content, "getCurrentStatus", "Current status for " + item->name() + " retrieved.");
}

void Daemon::setStatus(QString shortName, QString name, QString step, QString type, QString state, QString comment, int completionRatio, int version, bool published, QString user, QTcpSocket *client)
{
    log("I'm replying to this request: setStatus", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "setStatus", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    RamItem *item = nullptr;
    if (type == "A") item = qobject_cast<RamItem*>( proj->assets()->fromName(shortName, name) );
    else item = qobject_cast<RamItem*>( proj->shots()->fromName(shortName, name) );

    if (!item)
    {
        post(client, content, "setStatus", "Sorry, item not found. Check its name, short name and type", false);
        return;
    }

    // Get step from name
    RamObject *stepObj = proj->steps()->fromName( step );
    if(!stepObj)
    {
        post(client, content, "setStatus", "Sorry, step " + step + " not found.", false);
        return;
    }
    RamStep *stp = qobject_cast<RamStep*>( stepObj );

    // Get user
    RamObject *u = Ramses::instance()->users()->fromName(user);
    RamUser *usr;
    if (!u)
    {
        usr = Ramses::instance()->currentUser();
        if (!usr)
            usr = Ramses::instance()->ramsesUser();
    }
    else usr = qobject_cast<RamUser*>( u );

    // Get state
    RamObject *sttObj = Ramses::instance()->states()->fromName(state);
    RamState *stt;
    if (!sttObj)
        stt = Ramses::instance()->wipState();
    else stt = qobject_cast<RamState*>( sttObj );

    RamStatus *newStatus = item->setStatus(usr, stt, stp, completionRatio, comment, version);
    newStatus->setPublished(published);

    post(client, content, "setStatus", "Status updated!");
    return;

}

void Daemon::getAssets(QTcpSocket *client)
{
    log("I'm replying to this request: getAssets", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getAssets", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray assets;
    for (int i = 0; i < proj->assets()->count(); i++)
    {
        RamAsset *a = qobject_cast<RamAsset*>( proj->assets()->at(i) );
        QJsonObject asset = assetToJson(a);
        assets.append(asset);
    }
    content.insert("assets", assets);
    post(client, content, "getASsets", "Asset list retrieved.");
}

void Daemon::getAsset(QString shortName, QString name, QTcpSocket *client)
{
    log("I'm replying to this request: gatAsset", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    if (!proj)
    {
        post(client, QJsonObject(), "getCurrentStatus", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    RamAsset *asset = qobject_cast<RamAsset*>( proj->assets()->fromName(shortName, name) );
    if (!asset)
    {
        post(client, QJsonObject(), "getAsset", "Sorry, asset not found. Check its short name and name", false);
        return;
    }

    QJsonObject content = assetToJson(asset);
    post(client, content, "getASset", "Asset retrieved.");
}

void Daemon::getAssetGroups(QTcpSocket *client)
{
    log("I'm replying to this request: getAssetGroups", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getAssets", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray assetGroups;
    for( int i = 0; i < proj->assetGroups()->count(); i++)
    {
        RamAssetGroup *ag = qobject_cast<RamAssetGroup*>( proj->assetGroups()->at(i) );
        QJsonObject assetGroup;
        assetGroup.insert("shortName", ag->shortName());
        assetGroup.insert("name", ag->name());
        assetGroup.insert("folder", ag->path(RamObject::NoFolder, true));

        assetGroups.append(assetGroup);
    }
    content.insert("assetGroups", assetGroups);
    post(client, content, "getAssetGroups", "Asset group list retrieved.");
}

void Daemon::getProject(QString shortName, QString name, QTcpSocket *client)
{
    log("I'm replying to this request: getCurrentProject", DuQFLog::Debug);

    QJsonObject content;

    RamProject *proj = nullptr;

    if (shortName == "" && name == "")
    {
        proj = Ramses::instance()->currentProject();
    }
    else
    {
        proj = qobject_cast<RamProject*>( Ramses::instance()->projects()->fromName(shortName, name) );
    }

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
    content.insert("folder", proj->path(RamObject::NoFolder, true));

    post(client, content, "getCurrentProject", "Retrieved project: " + proj->name());
}

void Daemon::getCurrentUser(QTcpSocket *client)
{
    log("I'm replying to this request: getCurrentUser", DuQFLog::Debug);

    RamUser *user = Ramses::instance()->currentUser();

    QJsonObject content;

    if (!user)
    {
        post(client, content, "getCurrentUser", "Sorry, there's no current user. You need to log in first!", false);
        return;
    }

    content.insert("shortName", user->shortName());
    content.insert("name", user->name());
    content.insert("folderPath", user->path(RamObject::NoFolder, true));
    content.insert("comment", user->comment());
    RamUser::UserRole role = user->role();
    if (role == RamUser::Admin) content.insert("role", "ADMIN");
    else if (role == RamUser::ProjectAdmin) content.insert("role","PROJECT_ADMIN");
    else if (role == RamUser::Lead) content.insert("role", "LEAD");
    else content.insert("role", "STANDARD");

    post( client, content, "getCurrentUser", "Current user is: " + user->name());
}

void Daemon::getPipes(QTcpSocket *client)
{
    log("I'm replying to this request: getPipes", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getPipes", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray pipes;
    for (int i = 0; i < proj->pipeline()->count(); i++)
    {
        RamPipe *p = qobject_cast<RamPipe*>( proj->pipeline()->at(i));
        QJsonObject pipe;
        if (!p->inputStep()) continue;
        if (!p->outputStep()) continue;
        if (p->inputStep()->is(p->outputStep())) continue;
        pipe.insert("inputStepShortName", p->inputStep()->shortName());
        pipe.insert("outputStepShortName", p->outputStep()->shortName());
        QJsonArray pipeFiles;
        for( int j =0; j < p->pipeFiles()->count();j++)
        {
            QJsonObject pipeFile;
            RamPipeFile *pf = qobject_cast<RamPipeFile*>( p->pipeFiles()->at(j) );
            pipeFile.insert("colorSpace", "");
            pipeFile.insert("shortName", pf->shortName());
            pipeFile.insert("customSettings", pf->customSettings());
            QJsonObject fileType;
            RamFileType *ft = pf->fileType();
            if (ft)
            {
                fileType.insert("shortName", ft->shortName());
                fileType.insert("name", ft->name());
                QJsonArray extensions;
                for (int k = 0; k < ft->extensions().count(); k++)
                {
                    extensions.append( ft->extensions().at(k));
                }
                fileType.insert("extensions", extensions);
            }
            else
            {
                fileType.insert("shortName", "");
                fileType.insert("name", "");
                QJsonArray extensions;
                fileType.insert("extensions", extensions);
            }
            pipeFile.insert("fileType", fileType);
            pipeFiles.append(pipeFile);
            pipe.insert("pipeFiles", pipeFiles);
        }

        pipes.append(pipe);
    }
    content.insert("pipes", pipes);
    post(client, content, "getPipes", "Pipeline retrieved.");
}

void Daemon::getProjects(QTcpSocket *client)
{
    log("I'm replying to this request: getProjects", DuQFLog::Debug);

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
        project.insert("folder", p->path(RamObject::NoFolder, true));
        projects.append(project);
    }
    content.insert("projects", projects);

    post(client, content, "getProjects", "Project list retrived");
}

void Daemon::getSequences(QTcpSocket *client)
{
    log("I'm replying to this request: getSequences", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getSequences", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray sequences;
    for( int i = 0; i < proj->sequences()->count(); i++)
    {
        RamSequence *seq = qobject_cast<RamSequence*>( proj->sequences()->at(i) );
        QJsonObject sequence;
        sequence.insert("shortName", seq->shortName());
        sequence.insert("name", seq->name());

        sequences.append(sequence);
    }
    content.insert("sequences", sequences);
    post(client, content, "getSequences", "Sequence list retrieved.");
}

void Daemon::getShots(QString filter, QTcpSocket *client)
{
    log("I'm replying to this request: getShots", DuQFLog::Debug);

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
    for (int i = 0; i < proj->shots()->count(); i++)
    {
        RamShot *s = qobject_cast<RamShot*>( proj->shots()->at(i) );

        bool ok = filter == "([a-z0-9+-]*\\s*)*";
        if (!ok)
        {
            ok = re.match( s->shortName() ).hasMatch();
            if (!ok) ok = re.match( s->name() ).hasMatch();
        }
        if (!ok) continue;

        QJsonObject shot = shotToJson(s);
        shots.append(shot);
    }
    content.insert("shots", shots);
    post(client, content, "getShots", "Shot list retrieved.");
}

void Daemon::getShot(QString shortName, QString name, QTcpSocket *client)
{
    log("I'm replying to this request: getShot", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getShot", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    RamShot *shot = qobject_cast<RamShot*>( proj->shots()->fromName(shortName, name) );
    if (!shot)
    {
        post(client, QJsonObject(), "getShot", "Sorry, shot not found. Check its short name and name", false);
        return;
    }

    content = shotToJson(shot);
    post(client, content, "getShot", "Shot retrieved.");
}

void Daemon::getStates(QTcpSocket *client)
{
    log("I'm replying to this request: getStates", DuQFLog::Debug);

    QJsonObject content;
    QJsonArray states;
    RamStateList *ramStates = Ramses::instance()->states();
    for (int i = 0; i < ramStates->count(); i++)
    {
        RamState *s = qobject_cast<RamState*>( ramStates->at(i) );
        QJsonObject state = stateToJson(s);
        states.append(state);
    }
    content.insert("states", states);

    post(client, content, "getStates", "State list retrived");
}

void Daemon::getState(QString shortName, QString name, QTcpSocket *client)
{
    log("I'm replying to this request: getState", DuQFLog::Debug);

    QJsonObject content;

    RamState *state = qobject_cast<RamState*>( Ramses::instance()->states()->fromName( shortName, name ) );
    if (!state)
    {
        post(client, QJsonObject(), "getState", "Sorry, state not found. Check its short name and name", false);
        return;
    }

    content = stateToJson(state);
    post(client, content, "getState", "State retrieved.");
}

void Daemon::getSteps(QTcpSocket *client)
{
    log("I'm replying to this request: getSteps", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getSteps", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    QJsonArray steps;
    RamObjectList *projectSteps = proj->steps();
    for (int i = 0; i < projectSteps->count(); i++)
    {
        RamStep *s = qobject_cast<RamStep*>( projectSteps->at(i) );
        QJsonObject step = stepToJson(s);
        steps.append(step);
    }
    content.insert("steps", steps);
    post(client, content, "getSteps", "Step list retrieved.");
}

void Daemon::getStep(QString shortName, QString name, QTcpSocket *client)
{
    log("I'm replying to this request: getStep", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "getStep", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    RamStep *step = nullptr;
    RamObjectList *steps = proj->steps();
    for (int i = 0; i < steps->count(); i++)
    {
        RamStep *s = qobject_cast<RamStep*>( steps->at(i) );
        if (s->shortName() == shortName)
        {
            if (name == "" || s->name() == name)
            {
                step = s;
                break;
            }
        }
    }
    if (!step)
    {
        post(client, QJsonObject(), "getStep", "Sorry, step not found. Check its short name and name", false);
        return;
    }

    content = stepToJson(step);
    post(client, content, "getStep", "Step retrieved.");
}

void Daemon::getRamsesFolder(QTcpSocket *client)
{
    log("I'm replying to this request: getRamsesFolder", DuQFLog::Debug);

    QJsonObject content;
    content.insert("folder", Ramses::instance()->path(RamObject::NoFolder, true));
    post(client, content, "getRamsesFolder", "Ramses folder retrieved.");
}

void Daemon::setPublishSettings(QString stepShortName, QString stepName, QString settings, QTcpSocket *client)
{
    log("I'm replying to this request: setPublishSettings", DuQFLog::Debug);

    RamProject *proj = Ramses::instance()->currentProject();

    QJsonObject content;

    if (!proj)
    {
        post(client, content, "setPublishSettings", "Sorry, there's no current project. Select a project first!", false);
        return;
    }

    RamStep *step = qobject_cast<RamStep*>(proj->steps()->fromName(stepShortName, stepName));

    if (!step)
    {
        post(client, content, "setPublishSettings", "Sorry, step not found. Check its name and short name", false);
        return;
    }

    step->setPublishSettings(settings);
    step->update();

    post(client, content, "setPublishSettings", "Publish settings updated!");
}

Daemon::Daemon(QObject *parent) : DuQFLoggerObject("Daemon", parent)
{
    m_tcpServer = new QTcpServer(this);

    start();

    connect(m_tcpServer, &QTcpServer::newConnection, this, &Daemon::newConnection);
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

    log("Posting:\n" + jsonReply, DuQFLog::Data);
}

QJsonObject Daemon::assetToJson(RamAsset *asset)
{
    QJsonObject a;
    a.insert("shortName", asset->shortName());
    a.insert("name", asset->name());
    a.insert("folder", asset->path(RamObject::NoFolder, true));
    QJsonArray tags;
    foreach(QString t, asset->tags())
    {
        tags.append(t);
    }
    a.insert("tags", tags);

    RamProject *proj = Ramses::instance()->currentProject();
    if (proj)
    {
        RamAssetGroup *ag = asset->assetGroup();
        if (ag) a.insert("group", ag->name());
        else a.insert("group", "");
    }
    else a.insert("group", "");

    return a;
}

QJsonObject Daemon::shotToJson(RamShot *s)
{
    QJsonObject shot;
    shot.insert("shortName", s->shortName());
    shot.insert("name", s->name());
    shot.insert("folder", s->path(RamObject::NoFolder, true));
    shot.insert("duration", s->duration());
    shot.insert("sequence", s->sequence()->name());
    return shot;
}

QJsonObject Daemon::stateToJson(RamState *s)
{
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
    return state;
}

QJsonObject Daemon::stepToJson(RamStep *s)
{
    QJsonObject step;
    step.insert("shortName", s->shortName());
    step.insert("name", s->name());
    QJsonArray col;
    col.append( s->color().red()/255.0 );
    col.append( s->color().green()/255.0 );
    col.append( s->color().blue()/255.0 );
    step.insert("color", col);
    step.insert("comment", s->comment());
    step.insert("publishSettings", s->publishSettings());
    step.insert("folder", s->path(RamObject::NoFolder, true));
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
    return step;
}

QJsonObject Daemon::statusToJson(RamStatus *s)
{
    QJsonObject status;
    status.insert("step", s->step()->shortName() );
    status.insert("comment", s->comment() );
    status.insert("completionRatio", s->completionRatio() );
    status.insert("date", s->date().toString("yyyy-MM-dd hh:mm:ss"));
    if (s->state()) status.insert("state", s->state()->shortName() );
    if (s->user()) status.insert("user", s->user()->shortName() );
    status.insert("version", s->version() );
    return status;
}

