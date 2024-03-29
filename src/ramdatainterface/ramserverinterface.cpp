#include "ramserverinterface.h"

#include "datacrypto.h"
#include "duqf-app/app-version.h"
#include "progressmanager.h"
#include "ramdatainterface/localdatainterface.h"
#include "ramdatainterface/logindialog.h"
#include "duqf-utils/guiutils.h"
#include "statemanager.h"

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

int RamServerInterface::serverPort() const
{
    return m_serverPort;
}

void RamServerInterface::setServerPort(int newServerPort)
{
    m_serverPort = newServerPort;
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

bool RamServerInterface::isSyncing() const
{
    return m_syncing;
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
        emit userChanged("", "", "", "");
        return "";
    }

    QJsonObject repObj = parseData(reply);
    bool repSuccess = repObj.value("success").toBool();

    if (!repSuccess)
    {
        setConnectionStatus(NetworkUtils::Connecting, tr("Login failed."));
        // erase password
        eraseUserPassword();
        // Try again
        setOnline();
        return m_currentUserUuid;
    }

    // get the new token
    m_sessionToken = repObj.value("content").toObject().value("token").toString();
    setConnectionStatus(NetworkUtils::Online, tr("Server ready"));

    m_currentUserUuid = repObj.value("content").toObject().value("uuid").toString();

    if (m_currentUserUuid == "")
    {
        setConnectionStatus(NetworkUtils::Connecting, tr("Login failed."));
        // erase password
        eraseUserPassword();
        // Try again
        setOnline();
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

    qDebug() << "New user UUID after online login: " << m_currentUserUuid;

    // The data of the user
    QString un = repObj.value("content").toObject().value("username").toString();
    QString data = repObj.value("content").toObject().value("data").toString();
    QString modified = repObj.value("content").toObject().value("modified").toString();

    // Warn everyone we've logged in
    emit userChanged(m_currentUserUuid, un, data, modified);

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
    StateManager::State previousState = StateManager::i()->state();
    StateManager::i()->setState(StateManager::Connecting);

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
        StateManager::i()->setState(previousState);
        return;
    }

    // Wait for the dialog to return
    QEventLoop loop;

    // Show the login dialog (if both the username and password have not been saved)
    LoginDialog *dialog = new LoginDialog(GuiUtils::appMainWindow());
    dialog->setServerAddress( m_serverAddress );

    connect(dialog, &LoginDialog::loggedIn, this, &RamServerInterface::doLogin);
    connect(dialog, &LoginDialog::finished, &loop, &QEventLoop::quit);
    connect(dialog, &LoginDialog::finished, dialog, &LoginDialog::deleteLater);

    dialog->open();

    // Wait for the dialog to return
    loop.exec();

    StateManager::i()->setState(previousState);
}

void RamServerInterface::sync(SyncData syncData)
{
    // We should not already be in a sync
    if (m_syncing) return;

    // Save sync data
    m_syncingData = syncData;

    // Start session
    startSync();
}

void RamServerInterface::downloadData()
{
    qDebug() << ">>> Downloading data";

    QStringList tableNames = LocalDataInterface::instance()->tableNames();

    SyncData syncData;
    syncData.syncDate = "1818-05-05 00:00:00";

    for(int i = 0; i < tableNames.count(); i++)
    {
        syncData.tables.insert( tableNames.at(i), QSet<TableRow>());
    }

    // Create a request to get the data
    sync(syncData);
}

// PUBLIC SLOTS //

void RamServerInterface::setOnline(QString serverUuid)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(5);
    pm->setText(tr("Connecting to the Ramses Server..."));
    pm->increment();

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

    pm->setText(tr("Ping!"));
    pm->increment();

    // Ping
    Request r = buildRequest("ping", QJsonObject());
    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        qDebug() << "<<< Can't set online... Ping failed.";
        setConnectionStatus(NetworkUtils::Offline, tr("Server unavailable."));
        pm->setText("Connection failed. Switched to Offline mode.");
        return;
    }

    QJsonObject repObj = parseData(reply);

    if (!checkServerUuid(repObj.value("serverUuid").toString())) return;
    if (!checkPing(repObj)) return;

    m_localServerUuid = serverUuid;

    pm->setText(tr("Pong!"));
    pm->increment();

    // get the server version
    m_serverVersion = repObj.value("content").toObject().value("version").toString();
    // login!

    pm->setText(tr("Logging in..."));
    pm->increment();

    login();

    pm->increment();
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
    if (repObj.isEmpty()) {
        finishSync(true);
        return;
    }

    QString repQuery = repObj.value("query").toString();
    bool repSuccess = repObj.value("success").toBool();
    QString repMessage = repObj.value("message").toString();
    QJsonArray repLog = repObj.value("debug").toArray();
    QString serverUuid = repObj.value("serverUuid").toString();
    QJsonObject content = repObj.value("content").toObject();

    // Check server UUID
    if (!checkServerUuid(serverUuid)) {
        finishSync(true);
        return;
    }

    // Log
    for (int i = 0; i < repLog.count(); i++)
    {
        QJsonObject o = repLog.at(i).toObject();
        QString level = o.value("level").toString("INFO");
        QString message = o.value("message").toString("");
        DuQFLog::LogType l = DuQFLog::Information;
        if (level == "DATA") l = DuQFLog::Data;
        else if (level == "DEBUG" || level == "INFO") l = DuQFLog::Debug;
        else if (level == "WARNING") l = DuQFLog::Warning;
        else if (level == "CRITICAL") l = DuQFLog::Critical;
        else if (level == "FATAL") l = DuQFLog::Fatal;
        log(tr("Server log:") + " " + message, l);
    }

    if (!repSuccess) finishSync(true);

    // Parse specific data
    if (repQuery == "ping")
    {
        if (checkPing(repObj))
        {
            // If we're connecting, login!
            if (m_status == NetworkUtils::Connecting) login();
        }
    }
    else if (repQuery == "loggedout")
    {
        log(repMessage, DuQFLog::Warning);
        log(tr("The server logged you out."));
        finishSync(true);
        setConnectionStatus(NetworkUtils::Offline, tr("The server logged you out."));
    }
    else if (repQuery == "sync")
    {     
        // If sync is not successful
        if (!repSuccess)
        {
            QString reason = tr("The server sync (start sync session) was not successful.\nThis is probably a bug or a configuration error.\nPlease report bugs on %1").arg(URL_SOURCECODE);
            log(reason, DuQFLog::Warning);
            finishSync(true);
            return;
        }

        // Start pushing
        pushNext();
    }
    else if (repQuery == "push")
    {
        // If sync is not successful, stop syncing.
        if (!repSuccess)
        {
            QString reason = tr("The server sync (push) was not successful.\nThis is probably a bug or a configuration error.\nPlease report bugs on %1").arg(URL_SOURCECODE);
            log(reason, DuQFLog::Warning);
            finishSync(true);
            return;
        }
        // If it's commited, fetch
        if (content.value("commited").toBool())
        {
            fetch();
            return;
        }
        // Continue pushing
        pushNext();
    }
    else if (repQuery == "fetch")
    {
        // If sync is not successful, stop syncing.
        if (!repSuccess)
        {
            QString reason = tr("The server sync (fetch) was not successful.\nThis is probably a bug or a configuration error.\nPlease report bugs on %1").arg(URL_SOURCECODE);
            log(reason, DuQFLog::Warning);
            finishSync(true);
            return;
        }
        // Get fetch data
        m_fetchData.tableCount = content.value("tableCount").toInt();
        m_fetchData.tables = QSet<TableFetchData>();
        if (m_fetchData.tableCount == 0)
        {
            finishSync();
        }
        QJsonArray fetchedTables = content.value("tables").toArray();
        ProgressManager *pm = ProgressManager::instance();
        pm->setText(tr("Sync: fetching new data..."));
        for (int i = 0; i < fetchedTables.count(); i++)
        {
            QJsonObject fetchObj = fetchedTables.at(i).toObject();
            TableFetchData fetchData;
            fetchData.name = fetchObj.value("name").toString();
            fetchData.rowCount = fetchObj.value("rowCount").toInt();
            fetchData.pageCount = fetchObj.value("pageCount").toInt();
            fetchData.deleteCount = fetchObj.value("deleteCount").toInt();
            m_fetchData.tables.insert(fetchData);
            pm->addToMaximum(fetchData.pageCount);
        }
        // Start pulling
        pullNext();
    }
    else if (repQuery == "pull")
    {
        // If sync is not successful, stop syncing.
        if (!repSuccess)
        {
            QString reason = tr("The server sync (pull) was not successful.\nThis is probably a bug or a configuration error.\nPlease report bugs on %1").arg(URL_SOURCECODE);
            log(reason, DuQFLog::Warning);
            finishSync(true);
            return;
        }
        // Store new data
        QJsonArray rowsArray = content.value("rows").toArray();
        QJsonArray deletedArray = content.value("deleted").toArray();
        QString table = content.value("table").toString();
        QSet<TableRow> rows;
        QStringList deletedUuids;
        if (m_pullData.tables.contains(table)) rows = m_pullData.tables.value(table);
        for (int i = 0; i < rowsArray.count(); i++)
        {
            TableRow row;
            QJsonObject rowObj = rowsArray.at(i).toObject();
            row.uuid = rowObj.value("uuid").toString();
            row.data = rowObj.value("data").toString();

#ifdef DEBUG_DATA
                qDebug().noquote() << "<<< Received data";
                qDebug().noquote() <<"raw data:";
                qDebug().noquote() << rowObj.value("data");
                qDebug().noquote() <<"parsed data:";
                qDebug().noquote() << row.data;
                qDebug() << ">>>";
#endif


            row.modified = rowObj.value("modified").toString();
            row.removed = rowObj.value("removed").toInt();
            row.userName = rowObj.value("userName").toString();
            rows.insert(row);
        }
        for (int i = 0; i < deletedArray.count(); i++)
        {
            QString uuid = deletedArray.at(i).toString();
            deletedUuids << uuid;
        }
        m_pullData.tables.insert(table, rows);
        m_pullData.deletedUuids.insert(table, deletedUuids);
        // Next pull
        pullNext();
    }
    else if (repQuery == "setPassword")
    {
        if (!repSuccess)
        {
            QMessageBox::warning(
                        GuiUtils::appMainWindow(),
                        tr("Failed to set new password"),
                        repMessage
                        );
            return;
        }
        QMessageBox::information(
                    GuiUtils::appMainWindow(),
                    tr("Changed password"),
                    tr("The new password has correctly been set.")
                    );
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

bool RamServerInterface::checkPing(QJsonObject repObj)
{
    bool repSuccess = repObj.value("success").toBool();

    // If ping is not successful, set offline.
    if (!repSuccess)
    {
        QString reason = tr("The server is unavailable.\nPlease check your network.\n\n(server ping failed)");
        log(reason, DuQFLog::Warning);
        setConnectionStatus(NetworkUtils::Offline, reason);
        finishSync(true);
        return false;
    }

    // get the server version
    m_serverVersion = repObj.value("content").toObject().value("version").toString();

    emit pong(repObj.value("serverUuid").toString());
    return true;
}

bool RamServerInterface::checkServerUuid(QString uuid)
{
    m_serverUuid = uuid;
    if (m_localServerUuid != "" && m_serverUuid != "" && m_localServerUuid != m_serverUuid)
    {
        QString reason = tr("This server is not in sync with this local database.\n\n"
                            "This can happen if you try to connect to a new server from an existing database,\n"
                            "Of if the server has been updated, or its configuration changed.\n\n"
                            "Create a new database to connect to this server ( %1 ).").arg(
                                serverAddress()
                            );
        log(reason, DuQFLog::Critical);
        setConnectionStatus(NetworkUtils::Offline, tr("This server is not in sync with this local database."));
        return false;
    }
    return true;
}

void RamServerInterface::queueRequest(QString query, QJsonObject body)
{
    Request r = buildRequest(query, body);
    queueRequest(r);
    startQueue();
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
    int port = serverPort();

    QUrl url(protocol % address % "?" % query);
    url.setPort(port);

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

    QString repAll = QString::fromUtf8(reply->readAll());



    reply->deleteLater();
    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    QJsonObject repObj = repDoc.object();

    if (repObj.isEmpty())
    {
        repObj.insert("message","Received an invalid object");
        repObj.insert("accepted",false);
        repObj.insert("success",false);
        repObj.insert("error", true);
        repObj.insert("query", "unknown");
        log(tr("The server returned invalid data.\n"
               "This may be a misconfiguration of the server, a temporary network failure, or a bug."), DuQFLog::Warning);
    }

    QString repQuery = repObj.value("query").toString();
    QString repMessage = repObj.value("message").toString();

    log(repQuery + "\n" + repMessage + "\nContent:\n" + repAll, DuQFLog::Data);

    if (repObj.value("error").toBool(false)) return QJsonObject();

    bool repSuccess = repObj.value("success").toBool();

    // Log the rep message
    DuQFLog::LogType logLevel = DuQFLog::Debug;
    if (repQuery != "login" && !repSuccess) logLevel = DuQFLog::Warning;
    if (repMessage.startsWith("warning", Qt::CaseInsensitive)) logLevel = DuQFLog::Warning;
    log(repMessage, logLevel);

    return repObj;
}

const QString &RamServerInterface::currentUserUuid() const
{
    return m_currentUserUuid;
}

void RamServerInterface::setUserPassword(QString uuid, QString newPassword, QString currentPassword)
{
    QJsonObject body;
    body.insert("uuid", uuid);
    body.insert("newPassword", newPassword);
    body.insert("currentPassword", currentPassword);
    Request r = buildRequest("setPassword", body);
    postRequest(r);
}

void RamServerInterface::deleteData(QHash<QString, QSet<QString> > uuidsToDelete)
{
    QJsonObject body;
    QJsonArray tables;
    QHashIterator<QString, QSet<QString>> i(uuidsToDelete);
    while (i.hasNext()) {
        i.next();
        QJsonArray uuids;
        foreach(QString uuid, i.value()) {
            uuids.append(uuid);
        }
        QJsonObject table;
        table.insert("rows", uuids);
        table.insert("name", i.key());
        tables.append(table);
    }

    body.insert("tables", tables);
    Request r = buildRequest("clean", body);
    postRequest(r);
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

bool RamServerInterface::checkServer(QString hostName)
{
    // Make sure the host is available
    QHostInfo info = QHostInfo::fromName(hostName);
    if (info.error() != QHostInfo::NoError)
    {
        // Set offline
        this->setOffline();
        if (info.error() == QHostInfo::HostNotFound)
        {
            log(tr("The Ramses Server at '%1' is unreachable, sorry. Check your connection to the network.\n"
                   "I'm switching to offline mode.").arg(hostName),
                DuQFLog::Warning);
        }
        else
        {
            log(tr("Network or Ramses Server at '%1' unavailable, please check your connection.\n"
                   "I'm switching to offline mode.").arg(hostName),
                DuQFLog::Warning);
        }

        return false;
    }
    return true;
}

void RamServerInterface::startSync()
{
    qDebug() << "Server Interface: Starting Sync...";

    ProgressManager *pm = ProgressManager::instance();
    pm->setText(tr("Starting server sync session..."));
    pm->addToMaximum(m_syncingData.tables.count());

    queueRequest("sync");
    m_syncing = true;
    m_pullData.syncDate = QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss");
    m_pullData.tables = QHash<QString, QSet<TableRow>>();
    emit syncStarted();
}

void RamServerInterface::push(QString table, QSet<TableRow> rows, QString date, bool commit)
{
    qDebug() << "Server Interface: Pushing " << rows.count() << " rows to " << table;

    QJsonObject body;
    body.insert("table", table);
    QJsonArray rowsArray;
    foreach( TableRow row, rows)
    {
        QJsonObject rowObj;
        rowObj.insert("uuid", row.uuid);
        rowObj.insert("data", row.data);
        rowObj.insert("removed", row.removed);
        rowObj.insert("modified", row.modified);
        if (table == "RamUser") rowObj.insert("userName", row.userName);
        rowsArray.append(rowObj);
    }
    body.insert("rows", rowsArray);
    body.insert("previousSyncDate", date);
    body.insert("commit", commit);
    queueRequest("push", body);
}

void RamServerInterface::pushNext()
{
    // If the tables list is empty, we've pushed everything, commit
    if (m_syncingData.tables.isEmpty())
    {
        commit();
        return;
    }

    // Get a table and push its first 100 rows
    QString table = *m_syncingData.tables.keyBegin();
    QSet<TableRow> rows = m_syncingData.tables.value(table);
    QSet<TableRow> pushRows;
    while (pushRows.count() < m_requestMaxRows && !rows.isEmpty())
    {
        QSet<TableRow>::const_iterator i = rows.cbegin();
        pushRows.insert( *i );

#ifdef DEBUG_DATA
            qDebug().noquote() << "<<< SENT DATA";
            qDebug().noquote() << (*i).data;
            qDebug() << ">>>";
#endif

        rows.erase( i );
    }

    // Update rows in syncing data
    m_syncingData.tables[table] = rows;

    // If we've got all rows, remove the table from the data to sync
    if (rows.isEmpty())
    {
        ProgressManager *pm = ProgressManager::instance();
        pm->increment();
        pm->setText(tr("Uploading data to the server..."));
        m_syncingData.tables.remove(table);
    }

    // Push the rows
    push(table, pushRows, m_syncingData.syncDate, false);
}

void RamServerInterface::commit()
{
    qDebug() << "Server Interface: Commit!";

    push("", QSet<TableRow>(), "1818-05-05 00:00:00", true);
}

void RamServerInterface::fetch()
{
    qDebug() << "Server Interface: Fetching changes...";

    queueRequest("fetch");
}

void RamServerInterface::pull(QString table, int page)
{
    qDebug() << "Server Interface: Pulling page #" << page << " from " << table;

    QJsonObject body;
    body.insert("table", table);
    body.insert("page", page);
    queueRequest("pull", body);
}

void RamServerInterface::pullNext()
{
    if (m_fetchData.tables.isEmpty())
    {
        finishSync();
        return;
    }

    // Get a table and pull its next page
    bool finished = true;
    QSet<TableFetchData>::const_iterator i = m_fetchData.tables.constBegin();
    while(i != m_fetchData.tables.constEnd())
    {
        TableFetchData fetchData = *i;

        if (fetchData.pulled || fetchData.currentPage >= fetchData.pageCount)
        {
            i++;
            continue;
        }

        ProgressManager *pm = ProgressManager::instance();
        pm->increment();
        pm->setText(tr("Downloading new data from the server..."));

        finished = false;
        fetchData.currentPage++;
        pull( fetchData.name, fetchData.currentPage );
        if (fetchData.currentPage >= fetchData.pageCount ) fetchData.pulled = true;

        m_fetchData.tables.erase(i);
        m_fetchData.tables.insert(fetchData);
        break;
    }

    // Everything has been pulled
    if (finished) finishSync();
}

void RamServerInterface::finishSync(bool withError)
{
    qDebug() << "Server Interface: Finishing sync...";

    ProgressManager *pm = ProgressManager::instance();
    if (withError)
        pm->setText(tr("Sync finished with error."));
    else
        pm->setText("Sync OK!");

    // Emit result
    if (!withError) emit syncReady(m_pullData, m_serverUuid );

    // Finish
    m_syncing = false;
    // Make room
    m_syncingData = SyncData();
    m_fetchData = FetchData();
    m_pullData = SyncData();

    emit syncFinished();

    if (!withError) qDebug() << "Server Interface: Sync finished!";
    else qDebug() << "Server Interface: Sync error!";
}

void RamServerInterface::postRequest(Request r)
{
    QUrl url = r.request.url();
    QString test = url.host();
    if (!checkServer(test)) return;

    QNetworkReply *reply = m_network->post(r.request, r.body.toUtf8());

    // Log URL / GET
    log( "New request: " +  url.toString(QUrl::RemovePassword), DuQFLog::Debug);
    // Log POST body (in debug mode only!)
    #ifdef QT_DEBUG
    log("Request data: " + r.body, DuQFLog::Data);
    #endif

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(finished()), reply, SLOT(deleteLater()));
}

QNetworkReply *RamServerInterface::synchronousRequest(Request r)
{
    // Log URL / GET
    QUrl url = r.request.url();
    QString test = url.host();
    if (!checkServer(test)) return nullptr;

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


    // Create a loop to wait for the data
    QTimer timer;
    timer.setSingleShot(true);

    QNetworkReply *reply = m_synchronousNetwork->post(r.request, r.body.toUtf8());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this,SLOT(networkError(QNetworkReply::NetworkError)));

    timer.start( m_timeout );

    while ( reply->isRunning() )
    {
        if (!timer.isActive()) {
            reply->abort();
            reply->deleteLater();
            return nullptr;
        }
        qApp->processEvents();
    }

    return reply;
}
