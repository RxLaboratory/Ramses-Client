#include "dbinterface.h"

DBInterface *DBInterface::_instance = nullptr;

DBInterface *DBInterface::instance()
{
    if (!_instance) _instance = new DBInterface();
    return _instance;
}

void DBInterface::login(QString username, QString password)
{
    //request url
    QStringList q;
    q << "username=" + username;
    q << "password=" + generatePassHash(password);

    request("login", q);
}

void DBInterface::setOffline()
{
    setConnectionStatus(NetworkUtils::Offline);
}

void DBInterface::setOnline()
{
    //ping
    _status = NetworkUtils::Connecting;
    request("ping", QStringList(), false);
}

void DBInterface::getUsers()
{
    request("getUsers");
}

void DBInterface::updateUser(QString uuid, QString shortName, QString name, QString role, QString folderPath, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "role=" + role;
    q << "folderPath=" + folderPath;
    q << "comment=" + comment;

    request("updateUser", q);
}

void DBInterface::updateUserPassword(QString uuid, QString c, QString n)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "current=" + generatePassHash(c);
    q << "new=" + generatePassHash(n);

    request("updatePassword", q);
}

void DBInterface::createUser(QString shortName, QString name, QString uuid, QString password)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "password=" + generatePassHash(password);

    request("createUser", q);
}

void DBInterface::removeUser(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeUser", q);
}

void DBInterface::createProject(QString shortName, QString name, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;

    request("createProject", q);
}

void DBInterface::getProjects()
{
    request("getProjects");
}

void DBInterface::getProject(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("getProject", q);
}

void DBInterface::updateProject(QString uuid, QString shortName, QString name, int width, int height, double framerate, QString folderPath, QString comment, QDate deadline)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "folderPath=" + folderPath;
    q << "comment=" + comment;
    if (width > 0) q << "width=" + QString::number(width);
    if (height > 0) q << "height=" + QString::number(height);
    if (framerate > 0) q << "framerate=" + QString::number(framerate);
    if (deadline.isValid()) q << "deadline=" + deadline.toString("yyyy-MM-dd");

    request("updateProject", q);
}

void DBInterface::removeProject(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeProject", q);
}

void DBInterface::assignStep(QString stepUuid, QString projectUuid)
{
    QStringList q;
    q << "stepUuid=" + stepUuid;
    q << "projectUuid=" + projectUuid;

    request("assignStep", q);
}

void DBInterface::createTemplateStep(QString shortName, QString name, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;

    request("createTemplateStep", q);
}

void DBInterface::getTemplateSteps()
{
    request("getTemplateSteps");
}

void DBInterface::updateTemplateStep(QString uuid, QString shortName, QString name, QString type, QString comment, QColor color)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "type=" + type;
    q << "comment=" + comment;
    if (color.isValid()) q << "color=" + color.name();

    request("updateTemplateStep", q);
}

void DBInterface::removeTemplateStep(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeTemplateStep", q);
}

void DBInterface::setTemplateStepEstimations(QString uuid, QString method, float veryEasy, float easy, float medium, float hard, float veryHard)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "method=" + method;
    q << "veryEasy=" + QString::number(veryEasy);
    q << "easy=" + QString::number(easy);
    q << "medium=" + QString::number(medium);
    q << "hard=" + QString::number(hard);
    q << "veryHard=" + QString::number(veryHard);

    request("setTemplateStepEstimations", q);
}

void DBInterface::createTemplateAssetGroup(QString shortName, QString name, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;

    request("createTemplateAssetGroup", q);
}

void DBInterface::getTemplateAssetGroups()
{
    request("getTemplateAssetGroups");
}

void DBInterface::updateTemplateAssetGroup(QString uuid, QString shortName, QString name, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "comment=" + comment;

    request("updateTemplateAssetGroup", q);
}

void DBInterface::removeTemplateAssetGroup(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeTemplateAssetGroup", q);
}

void DBInterface::createStep(QString shortName, QString name, QString projectUuid, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "projectUuid=" + projectUuid;

    request("createStep", q);
}

void DBInterface::updateStep(QString uuid, QString shortName, QString name, QString type, QString comment, QColor color)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "type=" + type;
    q << "comment=" + comment;
    if (color.isValid()) q << "color=" + color.name();

    request("updateStep", q);
}

void DBInterface::moveStep(QString uuid, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "order=" + QString::number(order);

    request("moveStep", q);
}

void DBInterface::setStepOrder(QString uuid, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "order=" + QString::number(order);

    request("setStepOrder", q);
}

void DBInterface::setStepEstimations(QString uuid, QString method, float veryEasy, float easy, float medium, float hard, float veryHard, QString multiplyGroupUuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "method=" + method;
    q << "multiplyGroupUuid=" + multiplyGroupUuid;
    q << "veryEasy=" + QString::number(veryEasy);
    q << "easy=" + QString::number(easy);
    q << "medium=" + QString::number(medium);
    q << "hard=" + QString::number(hard);
    q << "veryHard=" + QString::number(veryHard);

    request("setStepEstimations", q);
}

void DBInterface::removeStep(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeStep" , q);
}

void DBInterface::assignUser(QString projectUuid, QString userUuid)
{
    QStringList q;
    q << "projectUuid=" + projectUuid;
    q << "userUuid=" + userUuid;

    request("assignUser", q);
}

void DBInterface::unassignUser(QString projectUuid, QString userUuid)
{
    QStringList q;
    q << "projectUuid=" + projectUuid;
    q << "userUuid=" + userUuid;

    request("unassignUser", q);
}

void DBInterface::assignApplication(QString stepUuid, QString applicationUuid)
{
    QStringList q;
    q << "stepUuid=" + stepUuid;
    q << "applicationUuid=" + applicationUuid;

    request("assignApplication", q);
}

void DBInterface::unassignApplication(QString stepUuid, QString applicationUuid)
{
    QStringList q;
    q << "stepUuid=" + stepUuid;
    q << "applicationUuid=" + applicationUuid;

    request("unassignApplication", q);
}

void DBInterface::createAssetGroup(QString shortName, QString name, QString projectUuid, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "projectUuid=" + projectUuid;

    request("createAssetGroup", q);
}

void DBInterface::updateAssetGroup(QString uuid, QString shortName, QString name, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "comment=" + comment;

    request("updateAssetGroup", q);
}

void DBInterface::removeAssetGroup(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeAssetGroup", q);
}

void DBInterface::createAsset(QString shortName, QString name, QString assetGroupUuid, QString tags, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "assetGroupUuid=" + assetGroupUuid;
    q << "tags=" + tags;

    request("createAsset", q);
}

void DBInterface::updateAsset(QString uuid, QString shortName, QString name, QString assetGroupUuid, QString tags, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "assetGroupUuid=" + assetGroupUuid;
    q << "tags=" + tags;
    q << "comment=" + comment;

    request("updateAsset", q);
}

void DBInterface::removeAsset(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeAsset", q);
}

void DBInterface::setAssetStatus(QString assetUuid, QString stateUuid, QString stepUuid, QString userUuid, int completionRatio, QString comment, int version, QString uuid, QString assignedUserUuid)
{
    QStringList q;
    q << "assetUuid=" + assetUuid;
    q << "stateUuid=" + stateUuid;
    q << "stepUuid=" + stepUuid;
    q << "userUuid=" + userUuid;
    q << "completionRatio=" + QString::number(completionRatio);
    q << "comment=" + comment;
    q << "version=" + QString::number(version);
    q << "uuid=" + uuid;
    q << "assignedUserUuid=" + assignedUserUuid;

    request("setAssetStatus", q);
}

void DBInterface::createSequence(QString shortName, QString name, QString projectUuid, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "projectUuid=" + projectUuid;

    request("createSequence", q);
}

void DBInterface::updateSequence(QString uuid, QString shortName, QString name, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "comment=" + comment;

    request("updateSequence", q);
}

void DBInterface::removeSequence(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeSequence", q);
}

void DBInterface::setSequenceOrder(QString uuid, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "order=" + QString::number(order);

    request("setSequenceOrder", q);
}

void DBInterface::createShot(QString shortName, QString name, QString sequenceUuid, QString uuid, qreal duration, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "sequenceUuid=" + sequenceUuid;
    q << "duration=" + QString::number(duration);
    if (order != -1) q << "order=" + QString::number(order);

    request("createShot", q);
}

void DBInterface::updateShot(QString uuid, QString shortName, QString name, QString sequenceUuid, qreal duration, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "sequenceUuid=" + sequenceUuid;
    q << "comment=" + comment;
    if (duration != -1) q << "duration=" + QString::number(duration);

    request("updateShot", q);
}

void DBInterface::moveShot(QString uuid, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "order=" + QString::number(order);

    request("moveShot", q);
}

void DBInterface::setShotOrder(QString uuid, int order)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "order=" + QString::number(order);

    request("setShotOrder", q);
}

void DBInterface::removeShot(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeShot", q);
}

void DBInterface::setShotStatus(QString shotUuid, QString stateUuid, QString stepUuid, QString userUuid, int completionRatio, QString comment, int version, QString uuid, QString assignedUserUuid)
{
    QStringList q;
    q << "shotUuid=" + shotUuid;
    q << "stateUuid=" + stateUuid;
    q << "stepUuid=" + stepUuid;
    q << "userUuid=" + userUuid;
    q << "completionRatio=" + QString::number(completionRatio);
    q << "comment=" + comment;
    q << "version=" + QString::number(version);
    q << "uuid=" + uuid;
    q << "assignedUserUuid=" + assignedUserUuid;

    request("setShotStatus", q);
}

void DBInterface::assignAsset(QString shotUuid, QString assetUuid)
{
    QStringList q;
    q << "uuid=" + shotUuid;
    q << "assetUuid=" + assetUuid;

    request("assignAsset", q);
}

void DBInterface::unassignAsset(QString shotUuid, QString assetUuid)
{
    QStringList q;
    q << "uuid=" + shotUuid;
    q << "assetUuid=" + assetUuid;

    request("unassignAsset", q);
}

void DBInterface::createState(QString shortName, QString name, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;

    request("createState", q);
}

void DBInterface::getStates()
{
    request("getStates");
}

void DBInterface::updateState(QString uuid, QString shortName, QString name, QString color, QString completionRatio, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "color=" + color.replace("#","%23");
    q << "completionRatio=" + completionRatio;
    q << "comment=" + comment;

    request("updateState", q);
}

void DBInterface::removeState(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeState", q);
}

void DBInterface::createFileType(QString shortName, QString name, QStringList extensions, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "extensions=" + extensions.join(",");

    request("createFileType", q);
}

void DBInterface::getFileTypes()
{
    request("getFileTypes");
}

void DBInterface::updateFileType(QString uuid, QString shortName, QString name, QStringList extensions, bool previewable, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "extensions=" + extensions.join(",");
    q << "comment=" + comment;
    if (previewable) q << "previewable=1";
    else q << "previewable=0";

    request("updateFileType", q);
}

void DBInterface::removeFileType(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeFileType", q);
}

void DBInterface::createApplication(QString shortName, QString name, QString executableFilePath, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "executableFilePath=" + executableFilePath;

    request("createApplication", q);
}

void DBInterface::getApplications()
{
    request("getApplications");
}

void DBInterface::updateApplication(QString uuid, QString shortName, QString name, QString executableFilePath, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "shortName=" + shortName;
    q << "name=" + name;
    q << "executableFilePath=" + executableFilePath;
    q << "comment=" + comment;

    request("updateApplication", q);
}

void DBInterface::removeApplication(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeApplication", q);
}

void DBInterface::assignFileType(QString applicationUuid, QString fileTypeUuid, QString type)
{
    QStringList q;
    q << "applicationUuid=" + applicationUuid;
    q << "fileTypeUuid=" + fileTypeUuid;
    q << "type=" + type;

    request("assignFileType", q);
}

void DBInterface::unassignFileType(QString applicationUuid, QString fileTypeUuid, QString type)
{
    QStringList q;
    q << "applicationUuid=" + applicationUuid;
    q << "fileTypeUuid=" + fileTypeUuid;
    q << "type=" + type;

    request("unassignFileType", q);
}

void DBInterface::createPipe(QString outputUuid, QString inputUuid, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "outputUuid=" + outputUuid;
    q << "inputUuid=" + inputUuid;

    request("createPipe", q);
}

void DBInterface::updatePipe(QString uuid, QString inputUuid, QString outputUuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "inputUuid=" + inputUuid;
    q << "outputUuid=" + outputUuid;

    request("updatePipe", q);
}

void DBInterface::removePipe(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removePipe", q);
}

void DBInterface::assignPipeFile(QString pipeUuid, QString pipeFileUuid)
{
    QStringList q;
    q << "pipeUuid=" + pipeUuid;
    q << "pipeFileUuid=" + pipeFileUuid;

    request("assignPipeFile", q);
}

void DBInterface::unassignPipeFile(QString pipeUuid, QString pipeFileUuid)
{
    QStringList q;
    q << "pipeUuid=" + pipeUuid;
    q << "pipeFileUuid=" + pipeFileUuid;

    request("unassignPipeFile", q);
}

void DBInterface::createPipeFile(QString shortName, QString projectUuid, QString fileTypeUuid, QString uuid, QString colorSpaceUuid)
{
    QStringList q;
    q << "shortName=" + shortName;
    q << "projectUuid=" + projectUuid;
    q << "fileTypeUuid=" + fileTypeUuid;
    q << "uuid=" + uuid;
    q << "colorSpaceUuid=" + colorSpaceUuid;

    request("createPipeFile", q);
}

void DBInterface::updatePipeFile(QString uuid, QString shortName, QString fileTypeUuid, QString colorSpaceUuid, QString comment)
{
    QStringList q;
    q << "shortName=" + shortName;
    q << "uuid=" + uuid;
    q << "fileTypeUuid=" + fileTypeUuid;
    q << "colorSpaceUuid=" + colorSpaceUuid;
    q << "comment=" + comment;

    request("updatePipeFile", q);
}

void DBInterface::removePipeFile(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removePipeFile", q);
}

void DBInterface::updateStatus(QString uuid, QString stateUuid, QString comment, int version, int completionRatio, bool published, QString assignedUserUuid, qint64 timeSpent, QDateTime date, QString difficulty, float estimation)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "stateUuid=" + stateUuid;
    q << "assignedUserUuid=" + assignedUserUuid;
    q << "difficulty=" + difficulty;
    q << "estimation=" + QString::number(estimation);
    if (timeSpent >= 0) q << "timeSpent=" + QString::number(timeSpent);
    if (published) q << "published=1";
    else q << "published=0";
    if (comment != "") q << "comment=" + comment;
    if (version > 0) q << "version=" + QString::number(version);
    if (completionRatio >= 0) q << "completionRatio=" + QString::number(completionRatio);
    if (date.isValid()) q << "date=" + date.toString("yyyy-MM-dd hh:mm:ss");


    request("updateStatus", q);
}

void DBInterface::setStatusUser(QString uuid, QString userUuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "userUuid=" + userUuid;

    request("setStatusUser", q);
}

void DBInterface::removeStatus(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeStatus", q);
}

void DBInterface::createSchedule(QString userUuid, QString stepUuid, QDateTime date, QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "userUuid=" + userUuid;
    q << "stepUuid=" + stepUuid;
    q << "date=" + date.toString("yyyy-MM-dd hh:mm:ss");

    request("createSchedule", q);
}

void DBInterface::createSchedules(QList<ScheduleEntryStruct> entries)
{
    if (entries.count() == 0) return;
    QJsonArray entriesArr;
    for (int i = 0; i < entries.count(); i++)
    {
        ScheduleEntryStruct e = entries.at(i);
        QJsonObject entry;
        entry.insert("uuid", e.uuid);
        entry.insert("stepUuid", e.stepUuid);
        entry.insert("userUuid", e.userUuid);
        entry.insert("date", e.date.toString("yyyy-MM-dd hh:mm:ss"));
        entriesArr.append(entry);
    }
    QJsonObject requestObj;
    requestObj.insert("entries", entriesArr);

    request("createSchedules", requestObj);
}

void DBInterface::updateSchedule(QString uuid, QString userUuid, QString stepUuid, QDateTime date, QString comment)
{
    QStringList q;
    q << "uuid=" + uuid;
    q << "userUuid=" + userUuid;
    q << "stepUuid=" + stepUuid;
    q << "comment=" + comment;
    q << "date=" + date.toString("yyyy-MM-dd hh:mm:ss");

    request("updateSchedule", q);
}

void DBInterface::updateSchedules(QList<ScheduleEntryStruct> entries)
{
    if (entries.count() == 0) return;
    QJsonArray entriesArr;
    for (int i = 0; i < entries.count(); i++)
    {
        ScheduleEntryStruct e = entries.at(i);
        QJsonObject entry;
        entry.insert("uuid", e.uuid);
        entry.insert("stepUuid", e.stepUuid);
        entry.insert("userUuid", e.userUuid);
        entry.insert("comment", e.comment);
        entry.insert("date", e.date.toString("yyyy-MM-dd hh:mm:ss"));
        entriesArr.append(entry);
    }
    QJsonObject requestObj;
    requestObj.insert("entries", entriesArr);

    request("updateSchedules", requestObj);
}

void DBInterface::removeSchedule(QString uuid)
{
    QStringList q;
    q << "uuid=" + uuid;

    request("removeSchedule", q);
}

void DBInterface::removeSchedules(QList<ScheduleEntryStruct> entries)
{
    if (entries.count() == 0) return;
    QJsonArray entriesArr;
    for (int i = 0; i < entries.count(); i++)
    {
        ScheduleEntryStruct e = entries.at(i);
        QJsonObject entry;
        entry.insert("uuid", e.uuid);
        entriesArr.append(entry);
    }
    QJsonObject requestObj;
    requestObj.insert("entries", entriesArr);

    request("removeSchedules", requestObj);
}

void DBInterface::init()
{
    request("init");
}

DBInterface::DBInterface(QObject *parent) : DuQFLoggerObject("Database Interface", parent)
{
    // LOCAL

    //Load local database
    _localDB = QSqlDatabase::addDatabase("QSQLITE","localdata");

    //TODO create local db etc

    // REMOTE

    _network.setCookieJar(new QNetworkCookieJar());
    _sessionToken = "";

    _suspended = false;

    _forbiddenWords << "and" << "or" << "if" << "else" << "insert" << "update" << "select" << "drop" << "alter";

    // Connect events
    connect( &_network, &QNetworkAccessManager::finished, this, &DBInterface::dataReceived);
    connect(&_network, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this,SLOT(sslError(QNetworkReply*,QList<QSslError>)));
    connect(qApp,SIGNAL(aboutToQuit()), this, SLOT(suspend()));

    _status = NetworkUtils::Offline;
}

bool DBInterface::isSuspended() const
{
    return _suspended;
}

void DBInterface::suspend(bool suspended)
{
    _suspended = suspended;
}

NetworkUtils::NetworkStatus DBInterface::connectionStatus() const
{
    return _status;
}

void DBInterface::dataReceived(QNetworkReply * rep)
{
    if (rep->error() != QNetworkReply::NoError) return;

    QString repAll = rep->readAll();

    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    QJsonObject repObj = repDoc.object();

    if (repObj.isEmpty())
    {
        repObj.insert("message",repAll);
        repObj.insert("accepted",false);
    }

    QString repQuery = repObj.value("query").toString();
    QString repMessage = repObj.value("message").toString();
    bool repSuccess = repObj.value("success").toBool();

    log(repQuery + "\n" + repMessage + "\nContent:\n" + repAll, DuQFLog::Data);

    if (!repSuccess)
    {
        log(repMessage, DuQFLog::Warning);
    }
    else
    {
        log(repMessage, DuQFLog::Information);
    }

    //if we recieved the reply from the ping, set online
    if (repQuery == "ping" && repSuccess) setConnectionStatus(NetworkUtils::Online);
    else if (repQuery == "ping") setConnectionStatus(NetworkUtils::Offline);

    //if login, get the token
    if (repQuery == "login" && repSuccess) _sessionToken = repObj.value("content").toObject().value("token").toString();
    else if (repQuery == "login") _sessionToken = "";

    emit data(repObj);
}

void DBInterface::sslError(QNetworkReply* /*rep*/, QList<QSslError> errs)
{
    foreach (QSslError err, errs)
    {
        log(err.errorString(), DuQFLog::Warning);
    }
    log("SSL Error. Connection may not be secured.", DuQFLog::Warning);
}

void DBInterface::networkError(QNetworkReply::NetworkError err)
{
    QString reason = "";
    if (err == QNetworkReply::ConnectionRefusedError)
    {
        reason = "The server refused the connection,\nplease try again later.";
    }
    else if (err == QNetworkReply::RemoteHostClosedError)
    {
        reason = "Server unavailable,\nplease try again later.";
    }
    else if (err == QNetworkReply::HostNotFoundError)
    {
        reason = "Invalid server,\ncheck the network settings.";
    }
    else if (err == QNetworkReply::TimeoutError)
    {
        reason = "Server unavailable,\nplease try again later.";
    }
    else if (err == QNetworkReply::OperationCanceledError)
    {
        reason = "Operation canceled.";
    }
    else if (err == QNetworkReply::SslHandshakeFailedError)
    {
        reason = "Secure connection (SSL) unavailable.\nCheck the network settings.";
    }
    else if (err == QNetworkReply::TemporaryNetworkFailureError)
    {
        reason = "Temporary network error,\nplease try again later.";
    }
    else if (err == QNetworkReply::NetworkSessionFailedError)
    {
        reason = "No connection found,\nplease check your network connection.";
    }
    else if (err == QNetworkReply::BackgroundRequestNotAllowedError)
    {
        reason = "Background request not allowed,\nplease check your system firewall settings.";
    }
#if QT_VERSION >= 0x050700
    else if (err == QNetworkReply::TooManyRedirectsError)
    {
        reason = "Too many redirections,\nthis server is misconfigured.";
    }
    else if (err == QNetworkReply::InsecureRedirectError)
    {
        reason = "Insecure redirection.";
    }
#endif
    else if (err == QNetworkReply::ProxyConnectionRefusedError)
    {
        reason = "The proxy refused the connection.";
    }
    else if (err == QNetworkReply::ProxyConnectionClosedError)
    {
        reason = "The proxy closed the connection prematurely.";
    }
    else if (err == QNetworkReply::ProxyNotFoundError)
    {
        reason = "The proxy was not found.\nPlease check your proxy settings.";
    }
    else if (err == QNetworkReply::ProxyTimeoutError)
    {
        reason = "The proxy did not respond in time.\nPlease check your proxy settings.";
    }
    else if (err == QNetworkReply::ProxyAuthenticationRequiredError)
    {
        reason = "The proxy needs authentication.";
    }
    else if (err == QNetworkReply::ContentAccessDenied)
    {
        reason = "The server denied the access (401).";
    }
    else if (err == QNetworkReply::ContentOperationNotPermittedError)
    {
        reason = "Operation not permitted.";
    }
    else if (err == QNetworkReply::ContentNotFoundError)
    {
        reason = "Content not found (404).\nCheck network settings.";
    }
    else if (err == QNetworkReply::AuthenticationRequiredError)
    {
        reason = "The server needs authentication.";
    }
    else if (err == QNetworkReply::ContentReSendError)
    {
        reason = "The request failed.\nPlease try again later.";
    }
    else if (err == QNetworkReply::ContentConflictError)
    {
        reason = "Content conflict.";
    }
    else if (err == QNetworkReply::ContentGoneError)
    {
        reason = "The requested resource is no longer available.";
    }
    else if (err == QNetworkReply::InternalServerError)
    {
        reason = "Internal server error.";
    }
    else if (err == QNetworkReply::OperationNotImplementedError)
    {
        reason = "The server cannot reply (operation not implemented).";
    }
    else if (err == QNetworkReply::ServiceUnavailableError)
    {
        reason = "Service unavailable.\nPlease try again later.";
    }
    else if (err == QNetworkReply::UnknownNetworkError)
    {
        reason = "Unknown network error.";
    }
    else if (err == QNetworkReply::UnknownProxyError)
    {
        reason = "Unknown proxy error.";
    }
    else if (err == QNetworkReply::UnknownContentError)
    {
        reason = "Unknown content error.";
    }
    else if (err == QNetworkReply::ProtocolFailure)
    {
        reason = "Protocol failure.";
    }
    else if (err == QNetworkReply::ProtocolFailure)
    {
        reason = "Unknown server error.";
    }
    setConnectionStatus( NetworkUtils::Offline );
    log(reason, DuQFLog::Critical);
}

void DBInterface::setConnectionStatus(NetworkUtils::NetworkStatus s)
{
    _status = s;
    if (s == NetworkUtils::Offline) _sessionToken = "";
    emit connectionStatusChanged(s);
}

bool DBInterface::waitPing()
{
    // If not online or connecting, we need to get online
    if (_status == NetworkUtils::Offline) setOnline();
    //wait three seconds when connecting or set offline
    int timeout = QSettings().value("server/timeout", 3000).toInt();
    QDeadlineTimer t(timeout);
    while (_status != NetworkUtils::Online)
    {
        qApp->processEvents();
        if ( t.hasExpired() || _status == NetworkUtils::Offline )
        {
            setOffline();
            log("Cannot process request, server unavailable.", DuQFLog::Critical);
            return false;
        }
    }
    return true;
}

QString DBInterface::getServerAddress()
{
    QSettings settings;
    //Get server address
    QString serverAddress = settings.value("server/address", "localhost/ramses/").toString();
    //if (!serverAddress.endsWith("/")) serverAddress += "/";
    return serverAddress;
}

QString DBInterface::getProtocol()
{
    QSettings settings;
    //Get server address
    QString protocol = "http://";
    if (settings.value("server/ssl", true).toBool())
    {
        protocol = "https://";
        if (!QSslSocket::supportsSsl()) {
            log("SSL is not available on this system. Please install OpenSSL to securely connect to the specified server.", DuQFLog::Critical);
            return "";
        }
    }
    return protocol;
}

void DBInterface::request(QString query, QStringList args, bool wait)
{
    if (_suspended) return;

    if (wait) if (!waitPing()) return;

    QString serverAddress = getServerAddress();
    QString protocol = getProtocol();
    if (protocol == "") return;

    QUrl url(protocol % serverAddress % "?" % query);
    QNetworkRequest request;
    request.setUrl(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));

    // Post body
    if (_sessionToken != "") args << "token=" + _sessionToken;
    QString body = buildFormEncodedString(args);

    // Send post
    _reply = _network.post(request, body.toUtf8());

#ifdef QT_DEBUG
    // Log URL / GET
    log( "New request: " + url.toString(QUrl::RemovePassword), DuQFLog::Debug);
    // Log POST body
    if (query == "login")
        log("Request data: [Hidden login info]", DuQFLog::Data);
    else
        log("Request data: " + body, DuQFLog::Data);
#endif

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
}

void DBInterface::request(QString query, QJsonObject body, bool wait)
{
    if (_suspended) return;

    if (wait) if (!waitPing()) return;

    QString serverAddress = getServerAddress();
    QString protocol = getProtocol();
    if (protocol == "") return;

    QUrl url(protocol % serverAddress % "?" % query);
    QNetworkRequest request;
    request.setUrl(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));

    // Post body

    if (_sessionToken != "") body.insert("token", _sessionToken );
    QJsonDocument bodyDoc;
    bodyDoc.setObject(body);

    // Send post
    _reply = _network.post(request, bodyDoc.toJson(QJsonDocument::Compact));

#ifdef QT_DEBUG
    // Log URL / GET
    log( "New request: " + url.toString(QUrl::RemovePassword), DuQFLog::Debug);
    // Log POST body
    if (query == "login")
        log("Request data: [Hidden login info]", DuQFLog::Data);
    else
        log("Request data: " + bodyDoc.toJson(), DuQFLog::Data);
#endif

    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
}

QString DBInterface::buildFormEncodedString(QStringList args)
{
    QString q;
    bool first = true;
    foreach(QString arg, args)
    {
        if (!first) q += "&";
        first = false;
        // Replace forbidden words
        foreach(QString word, _forbiddenWords)
        {
            arg = arg.replace(" " + word, "%" + word + "%");
        }
        q += QUrl::toPercentEncoding(arg, "=");
    }
    return q;
}

QString DBInterface::generatePassHash(QString password, QString salt)
{
    //hash password
    QString passToHash = password + salt;
    return QCryptographicHash::hash(passToHash.toUtf8(), QCryptographicHash::Sha3_512).toHex();
}
