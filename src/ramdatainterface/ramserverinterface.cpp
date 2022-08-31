#include "ramserverinterface.h"

#include "datacrypto.h"
#include "duqf-app/app-version.h"
#include "ramdatainterface/localdatainterface.h"
#include "ramdatainterface/logindialog.h"
#include "duqf-utils/guiutils.h"

// STATIC //

RamServerInterface *RamServerInterface::_instance = nullptr;

RamServerInterface *RamServerInterface::instance()
{
    if (!RamServerInterface::_instance) RamServerInterface::_instance = new RamServerInterface();
    return RamServerInterface::_instance;
}

// PUBLIC //

QString RamServerInterface::serverAddress() const
{
    return m_serverAddress;
}

void RamServerInterface::setServerAddress(QString newServerAddress)
{
    if (!newServerAddress.endsWith("/")) newServerAddress = newServerAddress + "/";
    m_serverAddress = newServerAddress;
}

bool RamServerInterface::ssl() const
{
    return m_ssl;
}

void RamServerInterface::setSsl(bool useSsl)
{
    if (m_ssl == useSsl) return;

    if (useSsl && !QSslSocket::supportsSsl()) {
        log("SSL is not available on this system. Please install OpenSSL to securely connect to the specified server.", DuQFLog::Critical);
        m_ssl = false;
    }
    else
    {
        m_ssl = useSsl;
    }
    emit sslChanged(m_ssl);
}

int RamServerInterface::timeOut() const
{
    return m_timeout;
}

void RamServerInterface::setTimeout(int newTimeout)
{
    m_timeout = newTimeout;
}

const QString &RamServerInterface::serverVersion() const
{
    return m_serverVersion;
}

NetworkUtils::NetworkStatus RamServerInterface::status() const
{
    return m_status;
}

bool RamServerInterface::isOnline() const
{
    switch(m_status)
    {
    case NetworkUtils::Online: return true;
    case NetworkUtils::Offline: return false;
    default:
        //Wait a sec or two
        QDeadlineTimer t(2000);
        while (m_status != NetworkUtils::Online && m_status != NetworkUtils::Offline)
        {
            qApp->processEvents();
            if (t.hasExpired()) break;
        }
        return m_status == NetworkUtils::Online;
    }
    return false;
}

// API

void RamServerInterface::ping()
{
    Request r = buildRequest("ping", QJsonObject());
    postRequest(r);
}

QString RamServerInterface::doLogin(QString username, QString password, bool saveUsername, bool savePassword)
{
    QJsonObject body;
    body.insert("username", username);
    body.insert("password", password);
    Request r = buildRequest("login", body);
    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        qDebug() << "<<< Can't log in... request timed out.";
        setConnectionStatus(NetworkUtils::Offline, tr("Unable to log in (request timed out)."));
        m_currentUserUuid = "";
        emit userChanged("");
        return "";
    }

    QJsonObject repObj = parseData(reply);
    bool repSuccess = repObj.value("success").toBool();

    if (!repSuccess)
    {
        QString reason = tr("Failed login");
        log(reason, DuQFLog::Warning);
        setConnectionStatus(NetworkUtils::Offline, reason);
        m_currentUserUuid = "";
        emit userChanged("");
        return "";
    }

    // get the new token
    m_sessionToken = repObj.value("content").toObject().value("token").toString();
    setConnectionStatus(NetworkUtils::Online, tr("Server ready"));

    m_currentUserUuid = repObj.value("content").toObject().value("uuid").toString();

    if (m_currentUserUuid == "")
    {
        setConnectionStatus(NetworkUtils::Connecting, "Login failed.");
        // Try again
        login();
        return m_currentUserUuid;
    }

    // Save credentials
    if (saveUsername)
    {
        QSettings settings;

        int serverIndex = 0;
        int historySize = settings.beginReadArray("servers");
        while (serverIndex < historySize)
        {
            settings.setArrayIndex(serverIndex);
            QString url = settings.value("url", "-").toString();
            if (url == m_serverAddress) break;
            serverIndex++;
        }
        settings.endArray();

        // Encrypt
        DataCrypto *crypto = DataCrypto::instance();
        username = crypto->machineEncrypt(username);
        if (savePassword) password = crypto->machineEncrypt(password);
        else password = "";

        settings.beginWriteArray("servers");
        settings.setArrayIndex(serverIndex);
        settings.setValue("url", m_serverAddress);
        settings.setValue("username", username);
        settings.setValue("password", password);
        settings.endArray();
    }

    // Warn everyone we've logged in
    emit userChanged(m_currentUserUuid);

    return m_currentUserUuid;
}

void RamServerInterface::eraseUserPassword()
{
    if (m_serverAddress == "" || m_serverAddress == "/") return;

    QSettings settings;

    int serverIndex = 0;
    int historySize = settings.beginReadArray("servers");
    while (serverIndex < historySize)
    {
        settings.setArrayIndex(serverIndex);
        QString url = settings.value("url", "-").toString();
        if (url == m_serverAddress) break;
        serverIndex++;
    }
    settings.endArray();

    settings.beginWriteArray("servers");
    settings.setArrayIndex(serverIndex);
    settings.setValue("password", "");
    settings.endArray();
}

void RamServerInterface::login()
{
    setConnectionStatus(NetworkUtils::Connecting, "Logging in...");

    // Check if we have saved credentials
    QSettings settings;
    int historySize = settings.beginReadArray("servers");
    QString username = "";
    QString password = "";
    for (int i = 0; i < historySize; i++)
    {
        settings.setArrayIndex(i);
        QString settingsAddress = settings.value("url").toString();
        if (settingsAddress == m_serverAddress )
        {
            // Decrypt
            DataCrypto *crypto = DataCrypto::instance();

            username = settings.value("username", "").toString();
            if (username != "") username = crypto->machineDecrypt( username );
            password = settings.value("password", "").toString();
            if (password != "") password = crypto->machineDecrypt( password );
        }
    }
    settings.endArray();

    if (username != "" && password != "")
    {
        doLogin(username, password, true, true);
        return;
    }

    // Wait for the dialog to return
    QEventLoop loop;

    // Show the login dialog (if both the username and password have not been saved)
    LoginDialog *dialog = new LoginDialog(GuiUtils::appMainWindow());
    dialog->setServerAddress( m_serverAddress );

    connect(dialog, &LoginDialog::loggedIn, this, &RamServerInterface::doLogin);
    connect(dialog, &LoginDialog::accepted, dialog, &LoginDialog::deleteLater);
    connect(dialog, &LoginDialog::rejected, dialog, &LoginDialog::deleteLater);
    connect(dialog, &LoginDialog::finished, &loop, &QEventLoop::quit);

    dialog->open();

    // Wait for the dialog to return
    loop.exec();
}

void RamServerInterface::sync(QJsonArray tables, QDateTime prevSyncDate)
{
    QJsonObject body;
    body.insert("tables", tables);
    body.insert("previousSyncDate", prevSyncDate.toString("yyyy-MM-dd hh:mm:00"));
    sync(body);
}

void RamServerInterface::sync(QJsonObject body)
{
    queueRequest("sync", body);
    startQueue();
}

QJsonArray RamServerInterface::downloadData()
{
    qDebug() << ">>> Downloading data";

    QStringList tableNames = LocalDataInterface::instance()->tableNames();
    QJsonArray tables;
    for(int i = 0; i < tableNames.count(); i++)
    {
        QJsonObject table;
        table.insert("name", tableNames.at(i));
        table.insert("modifiedRows", QJsonArray());
        tables << table;
    }

    // Create a request to get the data
    QJsonObject body;
    body.insert("tables", tables);
    body.insert("previousSyncDate", "1970-01-01 00:00:00");
    Request r = buildRequest("sync", body);

    // Post the request
    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        qDebug() << "<<< Can't download data... Request timed out.";
        return QJsonArray();
    }

    // Parse reply
    QJsonObject repObj = parseData(reply);

    qDebug() << "<<< Downloaded data: " << repObj;
    return repObj.value("content").toObject().value("tables").toArray();
}

// PUBLIC SLOTS //

void RamServerInterface::setOnline()
{
    if (m_serverAddress == "" || m_serverAddress == "/")
    {
        QMessageBox::information(
                    GuiUtils::appMainWindow(),
                    tr("No server"),
                    tr("I'm sorry, you need to configure a Ramses Server first.\n\n"
                       "Go to the database settings.")
                    );
        return;
    }

    setConnectionStatus(NetworkUtils::Connecting, "Server ping");

    // Ping
    Request r = buildRequest("ping", QJsonObject());
    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        qDebug() << "<<< Can't set online... Ping timed out.";
        setConnectionStatus(NetworkUtils::Offline, tr("Server unavailable (request timed out)."));
        emit userChanged("");
        return;
    }

    QJsonObject repObj = parseData(reply);

    bool repSuccess = repObj.value("success").toBool();

    // If ping is not successful, set offline.
    if (!repSuccess)
    {
        QString reason = tr("The server is unavailable.\nPlease check your network.\n\n(server ping failed)");
        log(reason, DuQFLog::Warning);
        setConnectionStatus(NetworkUtils::Offline, reason);
        emit userChanged("");
        return;
    }

    // get the server version
    m_serverVersion = repObj.value("content").toObject().value("version").toString();
    // login!
    login();

}

void RamServerInterface::setOffline()
{
    setConnectionStatus(NetworkUtils::Offline, "Switched to offline mode.");
}

// PRIVATE SLOTS //

void RamServerInterface::setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason)
{
    if (s == m_status) return;
    // Check security
    if (s != m_status && s == NetworkUtils::Online && !m_ssl) log(tr("The connection is not secured!"), DuQFLog::Critical);

    // update status
    m_status = s;
    // remove token if offline
    if (s == NetworkUtils::Offline) m_sessionToken = "";

    // log and emit signal
    if (s == NetworkUtils::Offline) log(tr("Disconnected:")  + " " + reason);
    else if (s == NetworkUtils::Connecting) log(tr("Connecting:")  + " " + reason);
    else if (s == NetworkUtils::Online) log(tr("Connected:")  + " " + reason);
    else if (s == NetworkUtils::Error) log(tr("Connection error:")  + " " + reason);

    emit connectionStatusChanged(s, reason);
}

void RamServerInterface::networkError(QNetworkReply::NetworkError err)
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
    log(tr("A network error has occured."), DuQFLog::Critical);
    log(reason, DuQFLog::Debug);
    setConnectionStatus( NetworkUtils::Error, tr("Network error:"));
    setConnectionStatus( NetworkUtils::Offline, tr("A network error has occured."));
}

void RamServerInterface::sslError(QNetworkReply *reply, QList<QSslError> errs)
{
    Q_UNUSED(reply);

    QString errors;
    foreach (QSslError err, errs)
    {
        log(err.errorString(), DuQFLog::Warning);
        errors = errors + "\n> " + err.errorString();
    }
    log(tr("An SSL Error has occured. The connection may not be secured."), DuQFLog::Warning);
    log(errors, DuQFLog::Debug);
    setConnectionStatus( NetworkUtils::Error, tr("SSL Errors") );
    setConnectionStatus( NetworkUtils::Offline, tr("There were SSL errors, the connection may not be secured.") );
}

void RamServerInterface::dataReceived(QNetworkReply *reply)
{
    QJsonObject repObj = parseData(reply);
    if (repObj.isEmpty()) return;

    QString repQuery = repObj.value("query").toString();
    bool repSuccess = repObj.value("success").toBool();
    QString repMessage = repObj.value("message").toString();

    // Parse specific data
    if (repQuery == "ping")
    {
        // If ping is not successful, set offline.
        if (!repSuccess)
        {
            QString reason = tr("The server is unavailable.\nPlease check your network.\n\n(server ping failed)");
            log(reason, DuQFLog::Warning);
            setConnectionStatus(NetworkUtils::Offline, reason);
            return;
        }

        // get the server version
        m_serverVersion = repObj.value("content").toObject().value("version").toString();
        // If we're connecting, login!
        if (m_status == NetworkUtils::Connecting) login();
    }
    else if (repQuery == "loggedout")
    {
        log(repMessage, DuQFLog::Warning);
        log(tr("The server logged you out."));
        setConnectionStatus(NetworkUtils::Offline, tr("The server logged you out."));
    }
    else if (repQuery == "sync")
    {
        // If sync is not successful, set offline.
        if (!repSuccess)
        {
            QString reason = tr("The server sync was not successful.\nThis is probably a bug or a configuration error.\nPlease report bugs on %1").arg(URL_SOURCECODE);
            log(reason, DuQFLog::Warning);
            setConnectionStatus(NetworkUtils::Offline, reason);
            return;
        }

        emit syncReady(repObj.value("content").toObject().value("tables").toArray());
    }
}

void RamServerInterface::nextRequest()
{
    if (m_requestQueue.isEmpty())
    {
        pauseQueue();
        return;
    }

    if (m_status != NetworkUtils::Online) {
        // If offline, stops the queue.
        if (m_status == NetworkUtils::Offline) pauseQueue();
        return; // Return now if we're not online
    }

    postRequest( m_requestQueue.takeFirst() );
}

void RamServerInterface::queueRequest(QString query, QJsonObject body)
{
    Request r = buildRequest(query, body);
    queueRequest(r);
}

void RamServerInterface::queueRequest(Request r)
{
    m_requestQueue << r;
    startQueue();
}

Request RamServerInterface::buildRequest(QString query, QJsonObject body)
{
    Request r = buildRequest(query);

    // Update header to JSON type
    r.request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Post body
    // Add version
    body.insert("version", STR_VERSION);
    // Current token
    if (m_sessionToken != "") body.insert("token", m_sessionToken );
    // Build body
    QJsonDocument bodyDoc;
    bodyDoc.setObject(body);
    r.body = bodyDoc.toJson(QJsonDocument::Compact);

    return r;
}

Request RamServerInterface::buildRequest(QString query)
{
    // Get info to build the URL
    QString address = serverAddress();
    QString protocol = serverProtocol();

    QUrl url(protocol % address % "?" % query);

    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));

    Request r;
    r.request = request;
    r.query = query;
    return r;
}

void RamServerInterface::flushRequests()
{
    if ( m_requestQueue.isEmpty() ) return;

    log( "Flushing remaining requests." );

    pauseQueue();

    // Post the remaining requests
    while( !m_requestQueue.isEmpty() )
    {
        nextRequest();
        qDebug() << m_requestDelay;
        qApp->processEvents();
        // Wait a small bit
        QThread::msleep( m_requestDelay / 4 );
    }

    // Wait a last bit to be sure everything is sent
    QThread::msleep( m_requestDelay );

    log( "All requests sent." );
}

void RamServerInterface::startQueue()
{
    if (m_status != NetworkUtils::Online) return;
    if (!m_requestQueueTimer->isActive()) m_requestQueueTimer->start(m_requestDelay);
}

void RamServerInterface::pauseQueue()
{
    m_requestQueueTimer->stop();
}

QJsonObject RamServerInterface::parseData(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        QJsonObject repObj;
        repObj.insert("error", true);
        repObj.insert("message", tr("A server error has occured."));
        repObj.insert("accepted",false);
        repObj.insert("success",false);
    }

    QString repAll = reply->readAll();
    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    QJsonObject repObj = repDoc.object();

    if (repObj.isEmpty())
    {
        repObj.insert("message","Received an invalid object");
        repObj.insert("accepted",false);
        repObj.insert("success",false);
        repObj.insert("error", true);
        repObj.insert("query", "unknown");
    }

    QString repQuery = repObj.value("query").toString();
    QString repMessage = repObj.value("message").toString();

    log(repQuery + "\n" + repMessage + "\nContent:\n" + repAll, DuQFLog::Data);

    if (repObj.value("error").toBool(false)) return QJsonObject();

    bool repSuccess = repObj.value("success").toBool();

    // Log the rep message
    DuQFLog::LogType logLevel = DuQFLog::Information;
    if (repQuery != "login" && !repSuccess) logLevel = DuQFLog::Warning;
    if (repMessage.startsWith("warning", Qt::CaseInsensitive)) logLevel = DuQFLog::Warning;
    log(repMessage, logLevel);

    return repObj;
}

const QString &RamServerInterface::currentUserUuid() const
{
    return m_currentUserUuid;
}

// PRIVATE //

RamServerInterface::RamServerInterface():
    DuQFLoggerObject("Ramses Server Interface", nullptr)
{
    // Prepare network connection
    QNetworkCookieJar *cookies = new QNetworkCookieJar();
    m_network = new QNetworkAccessManager();
    m_network->setCookieJar(cookies);
    m_network->setStrictTransportSecurityEnabled(true);
    m_synchronousNetwork = new QNetworkAccessManager();
    m_synchronousNetwork->setCookieJar(cookies);
    m_synchronousNetwork->setStrictTransportSecurityEnabled(true);

    m_forbiddenWords << "and" << "or" << "if" << "else" << "insert" << "update" << "select" << "drop" << "alter";

    // Don't post too many request at the same time
    m_requestQueueTimer = new QTimer(this);

    connectEvents();
}

void RamServerInterface::connectEvents()
{
    connect(m_requestQueueTimer, &QTimer::timeout, this, &RamServerInterface::nextRequest);
    connect(m_network, &QNetworkAccessManager::finished, this, &RamServerInterface::dataReceived);
    connect(m_network, &QNetworkAccessManager::sslErrors, this, &RamServerInterface::sslError);
    connect(qApp, &QApplication::aboutToQuit, this, &RamServerInterface::flushRequests);
}

const QString RamServerInterface::serverProtocol()
{
    if (m_ssl)
    {
        if (!QSslSocket::supportsSsl()) {
            log("SSL is not available on this system. Please install OpenSSL to securely connect to the specified server.", DuQFLog::Critical);
            return "http://";
        }
        else return "https://";
    }

    return "http://";
}

void RamServerInterface::postRequest(Request r)
{
    QNetworkReply *reply = m_network->post(r.request, r.body.toUtf8());
    QUrl url = r.request.url();

    // Log URL / GET
    log( "New request: " +  url.toString(QUrl::RemovePassword), DuQFLog::Debug);
    // Log POST body
    if (r.query == "login")
        #ifdef QT_DEBUG
        log("Request data: " + r.body, DuQFLog::Data);
        #else
        log("Request data: [Hidden login info]", DuQFLog::Data);
        #endif
    else
        log("Request data: " + r.body, DuQFLog::Data);

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkReply *RamServerInterface::synchronousRequest(Request r)
{
    // Log URL / GET
    QUrl url = r.request.url();
    log( "New request: " +  url.toString(QUrl::RemovePassword), DuQFLog::Debug);
    // Log POST body
    if (r.query == "login")
        #ifdef QT_DEBUG
        log("Request data: " + r.body, DuQFLog::Data);
        #else
        log("Request data: [Hidden login info]", DuQFLog::Data);
        #endif
    else
        log("Request data: " + r.body, DuQFLog::Data);


    // Create an eventloop to wait for the data
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    connect(&timer, &QTimer::timeout, &loop, [&]{ exit(1); } );

    QNetworkReply *reply = m_synchronousNetwork->post(r.request, r.body.toUtf8());

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);

    timer.start( m_timeout );

    // Wait for it
    if (loop.exec() == 0) timer.stop();
    else return nullptr;

    return reply;
}

























