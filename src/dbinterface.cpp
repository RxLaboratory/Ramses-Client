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
    QString requestString = "?type=login";

    QJsonObject obj;
    obj.insert("username",username);
    obj.insert("password", generatePassHash(password, username) );
    QJsonDocument json(obj);

    _status = NetworkUtils::Connecting;
    emit statusChanged(_status,"Connecting..");
    emit log("Connecting...", LogUtils::Remote);
    request(requestString, json);
}

void DBInterface::setOffline()
{
    _status = NetworkUtils::Offline;
    emit statusChanged(_status, "Got offline");
}

void DBInterface::getUsers()
{
    QString q = "?type=getUsers";
    request(q);
}

void DBInterface::updateUser(QString uuid, QString shortName, QString name)
{
    QString q = "?type=updateUser";
    QJsonObject obj;
    obj.insert("uuid",uuid);
    obj.insert("name",name);
    obj.insert("shortName",shortName);
    QJsonDocument json(obj);

    request(q,json);
}

void DBInterface::updateUserPassword(QString uuid, QString username, QString c, QString n)
{
    QString q = "?type=updatePassword";
    QJsonObject obj;
    obj.insert("uuid",uuid);
    obj.insert("current",generatePassHash(c, username));
    obj.insert("new",generatePassHash(n, username));
    QJsonDocument json(obj);

    request(q,json);
}

DBInterface::DBInterface(QObject *parent) : QObject(parent)
{
    // LOCAL

    //Load local database
    _localDB = QSqlDatabase::addDatabase("QSQLITE","localdata");

    //TODO create local db etc

    // REMOTE

    _network.setCookieJar(new QNetworkCookieJar());

    // Connect events
    connect( &_network, &QNetworkAccessManager::finished, this, &DBInterface::dataReceived);
    connect(&_network, SIGNAL(sslErrors(QNetworkReply *,QList<QSslError>)), this,SLOT(sslError(QNetworkReply *,QList<QSslError>)));


    _status = NetworkUtils::Offline;
}

void DBInterface::dataReceived(QNetworkReply * rep)
{
    if (rep->error() != QNetworkReply::NoError) return;

    if (_status != NetworkUtils::Online)
    {
        _status = NetworkUtils::Online;
        emit statusChanged(_status, "Online");
    }

    QString repAll = rep->readAll();

    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    QJsonObject repObj = repDoc.object();

    if (repObj.isEmpty())
    {
        repObj.insert("message",repAll);
        repObj.insert("accepted",false);
    }

    QString repType = repObj.value("type").toString();
    QString repMessage = repObj.value("message").toString();
    bool repSuccess = repObj.value("success").toBool();

    emit log(repType + "\n" + repMessage + "\nContent:\n" + repAll, LogUtils::Remote);

    if (!repSuccess)
    {
        emit log(repMessage, LogUtils::Warning);
        return;
    }
    else
    {
        emit log(repMessage, LogUtils::Information);
    }

    emit data(repObj);
}

void DBInterface::sslError(QNetworkReply* /*rep*/, QList<QSslError> errs)
{
    foreach (QSslError err, errs)
    {
        emit log(err.errorString(), LogUtils::Warning);
    }
    emit log("SSL Error. Connection may not be secured.", LogUtils::Warning);
}

void DBInterface::networkError(QNetworkReply::NetworkError err)
{
    QString reason;
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
    _status = NetworkUtils::Offline;
    emit statusChanged(_status, reason);
    emit log(reason, LogUtils::Critical);
}

void DBInterface::request(QString req, QJsonDocument content)
{
    QSettings settings;

    QString protocol = "http://";
    if (settings.value("server/ssl", true).toBool()) protocol = "https://";

    QString serverAddress = settings.value("server/address", "localhost/ramses/").toString();

    //request
    emit log("New request: " + protocol + serverAddress + req, LogUtils::Remote);
    QUrl url(protocol + serverAddress + req);
    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));
    if (content.isEmpty())
    {
        _reply = _network.get(request);
    }
    else
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
        if (req.indexOf("login") >= 0)
        {
#ifdef QT_DEBUG
            emit log("Request Content: " + content.toJson(), LogUtils::Remote);
#else
            emit log("Request Content: [hidden login informations]", LogUtils::Remote);
#endif
        }
        else
        {
            emit log("Request Content: " + content.toJson(), LogUtils::Remote);
        }
        _reply = _network.post(request,content.toJson());
    }
    connect(_reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
}

QString DBInterface::generatePassHash(QString password, QString salt)
{
    //hash password
    QString passToHash = password + salt;
    return QCryptographicHash::hash(passToHash.toUtf8(), QCryptographicHash::Sha3_512).toHex();
}
