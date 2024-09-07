#include "ramserverclient.h"

#include <QNetworkCookieJar>
#include <QApplication>
#include <QHostInfo>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>

#include "duapp/app-version.h"
#include "datacrypto.h"
#include "localdatainterface.h"

RamServerClient *RamServerClient::_instance = nullptr;

RamServerClient *RamServerClient::i()
{
    if (!_instance)
        _instance = new RamServerClient();
    return _instance;
}

ServerConfig RamServerClient::serverConfig() const
{
    ServerConfig c;
    c.address = serverAddress();
    c.timeout = timeOut();
    c.useSsl = ssl();
    c.port = serverPort();
    return c;
}

void RamServerClient::setServerSettings(ServerConfig settings)
{
    setServerAddress(settings.address);
    setTimeout(settings.timeout);
    setSsl(settings.useSsl);
    setServerPort(settings.port);
}

void RamServerClient::setServerAddress(const QString &newServerAddress)
{
    m_serverAddress = newServerAddress;
    if (!m_serverAddress.endsWith("/"))
        m_serverAddress += "/";
}

void RamServerClient::setSsl(bool useSsl)
{
    if (m_ssl == useSsl)
        return;

    if (useSsl && !QSslSocket::supportsSsl()) {
        qCritical().noquote() << "{Client}" << tr("SSL is not available on this system. Please install OpenSSL to securely connect to the specified server.");
        m_ssl = false;
    }
    else
    {
        m_ssl = useSsl;
    }
    emit sslChanged(m_ssl);
}

QString RamServerClient::serverProtocol() const
{
    if (m_ssl)
    {
        if (!QSslSocket::supportsSsl()) {
            qCritical().noquote() << "{Client}" << tr("SSL is not available on this system. Please install OpenSSL to securely connect to the specified server.");
            return "http://";
        }
        else return "https://";
    }

    return "http://";
}

void RamServerClient::setOffline()
{
    setStatus(Offline);
}

QJsonObject RamServerClient::ping(bool synchronous)
{
    Request r = buildRequest("ping", QJsonObject());

    if (!synchronous) {
        postRequest(r);
        return QJsonObject();
    }

    QJsonObject obj;
    obj.insert("success", false);

    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        qCritical().noquote() << "{Client}" << "Can't contact the server... Ping failed.";
        obj.insert("message", "Connection failed, server unavailable or misconfigured.");
        return obj;
    }

    obj = parseData(reply);
    if (!obj.value("success").toBool(false)) {
        m_lastError = obj.value("message").toString("Unknown error");
        qCritical().noquote() << "{Client}" << m_lastError;
        setStatus(Offline);
        return obj;
    }

    setStatus(Online);

    return obj;
}

QJsonObject RamServerClient::login(const QString &email, const QString &password)
{
    QJsonObject obj;
    obj.insert("success", false);

    // Checks

    if (email == "") {
        obj.insert("messsage", "The email is missing.");
        return obj;
    }

    if (password == "") {
        obj.insert("message", "The password is missing.");
        return obj;
    }

    // Ping

    if (m_status == Offline) {
        obj = ping(true);
        if (!obj.value("success").toBool(false))
            return obj;
    }

    // Hash password

    QString hashedPassword = hashPassword(password);

    QJsonObject body;
    body.insert("email", email);
    body.insert("password", hashedPassword);
    Request r = buildRequest("login", body);
    QNetworkReply *reply = synchronousRequest(r);

    if (!reply)
    {
        m_lastError = tr("Can't log in... request timed out.");
        qWarning().noquote() << "{Client}" << m_lastError;
        setStatus(Offline);
        obj.insert("success", false);
        obj.insert("message", "Unable to log in (the request timed out). Check your internet connection and the server address.");
        return obj;
    }

    obj = parseData(reply);
    bool repSuccess = obj.value("success").toBool();

    if (!repSuccess)
    {
        m_lastError = obj.value("message").toString("Login failed.");
        setStatus(Offline);
        qWarning().noquote() << "{Client}" << m_lastError;
        return obj;
    }

    // get the new token
    m_sessionToken = obj.value("content").toObject().value("token").toString();
    setStatus(Ready);
    qInfo().noquote() << "{Client}" << "Server ready!";

    return obj;
}

QJsonObject RamServerClient::setUserRole(const QString &uuid, const QString &role)
{
    QJsonObject body;
    body.insert("user", uuid);
    body.insert("role", role);

    Request r = buildRequest("setUserRole", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::createProject(const QString &data, const QString &uuid)
{
    QJsonObject body;
    body.insert("uuid", uuid);
    body.insert("data", data);

    Request r = buildRequest("createProject", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::setProject(const QString &uuid)
{
    QJsonObject body;
    body.insert("project", uuid);

    Request r = buildRequest("setCurrentProject", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::assignUsers(const QStringList &userUuids, const QString &projectUuid)
{
    QJsonObject body;
    body.insert("project", projectUuid);

    QJsonArray usersArr;
    for (const auto &uuid: userUuids)
        usersArr.append(uuid);
    body.insert("users", usersArr);

    Request r = buildRequest("assignUsers", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::getAllUsers()
{
    Request r = buildRequest("getAllUsers");

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::setPassword(const QString &userUuid, const QString &newPassword, const QString &currentPassword)
{
    QJsonObject body;
    body.insert("uuid", userUuid);
    body.insert("newPassword", newPassword);
    body.insert("currentPassword", currentPassword);

    Request r = buildRequest("setPassword", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::getEmail(const QString &uuid)
{
    QJsonObject body;
    body.insert("uuid", uuid);

    Request r = buildRequest("getEmail", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::setEmail(const QString &uuid, const QString &email)
{
    QJsonObject body;
    body.insert("uuid", uuid);
    body.insert("email", email);

    Request r = buildRequest("setEmail", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

QJsonObject RamServerClient::createUsers(const QJsonArray &users)
{
    QJsonObject body;
    body.insert("users", users);

    Request r = buildRequest("createUsers", body);

    QNetworkReply *reply = synchronousRequest(r);
    return parseData(reply);
}

void RamServerClient::sync(SyncData syncData)
{
    // We should not already be in a sync or logged out
    if (m_status != Ready) return;

    // Save sync data
    m_pushingData = syncData;

    // Start session
    startSync();
}

void RamServerClient::downloadAllData()
{
    if (m_status == Syncing)
        return;

    QStringList tableNames = LocalDataInterface::instance()->tableNames();

    m_pushingData = SyncData();
    m_pushingData.syncDate = "1818-05-05 00:00:00";

    for(int i = 0; i < tableNames.count(); i++)
    {
        m_pushingData.tables.insert( tableNames.at(i), QSet<TableRow>());
    }

    // Create a request to get the data

    startSync();
}

void RamServerClient::nextRequest()
{
    if (m_requestQueue.isEmpty())
    {
        pauseQueue();
        return;
    }

    if (m_status == ClientStatus::Offline) {
        pauseQueue();
        return; // Return now if we're not online
    }

    postRequest( m_requestQueue.takeFirst() );
}

void RamServerClient::dataReceived(QNetworkReply *reply)
{
    QJsonObject repObj = parseData(reply);
    if (repObj.isEmpty()) {
        finishSync(true);
        return;
    }

    QString repQuery = repObj.value("query").toString();
    bool repSuccess = repObj.value("success").toBool();
    QJsonArray repLog = repObj.value("debug").toArray();
    QJsonObject content = repObj.value("content").toObject();

    // Log
    for (int i = 0; i < repLog.count(); i++)
    {
        QJsonObject o = repLog.at(i).toObject();
        QString level = o.value("level").toString("INFO");
        QString message = o.value("message").toString("");

        if (level == "WARNING" || level == "CRITICAL" || level == "FATAL")
            qWarning().noquote() << "{Client}" << "Server Error | " + level + " | " + message;
#ifdef QT_DEBUG
        else if (level == "DATA")
            qDebug().noquote() << "{Client}" << message;
#endif
        else if (level == "DEBUG")
            qDebug().noquote() << "{Client}" << message;
    }

    if (!repSuccess) {
        qWarning().noquote() << "{Client}" << repObj.value("message").toString("Unknown error");
        finishSync(true);
    }

    // Parse specific data
    if (repQuery == "ping")
    {
        if (!repSuccess)
        {
            setStatus(Offline);
            m_lastError = "The server is unavailable.\nPlease check your network.\n\n(server ping failed)";
            qWarning().noquote() << "{Client}" << m_lastError;
        }
    }
    else if (repQuery == "loggedout")
    {
        setStatus(Offline);
        qWarning().noquote() << "{Client}" <<"The server logged you out.";
        finishSync(true);
    }
    else if (repQuery == "sync")
    {
        // If sync is not successful
        if (!repSuccess)
        {
            m_lastError = tr( "The server sync (start sync session) was not successful.\n"
                              "This is probably a bug or a configuration error.\n"
                              "Please report bugs on %1").arg(URL_SOURCECODE);
            qWarning().noquote() << "{Client}" << m_lastError;
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
            m_lastError = tr("The server sync (push) was not successful.\n"
                             "This is probably a bug or a configuration error.\n"
                             "Please report bugs on %1").arg(URL_SOURCECODE);
            qWarning().noquote() << "{Client}" << m_lastError;
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
            m_lastError =  tr("The server sync (fetch) was not successful.\n"
                             "This is probably a bug or a configuration error.\n"
                             "Please report bugs on %1").arg(URL_SOURCECODE);
            qWarning().noquote() << "{Client}" << m_lastError;
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
        for (int i = 0; i < fetchedTables.count(); i++)
        {
            QJsonObject fetchObj = fetchedTables.at(i).toObject();
            TableFetchData fetchData;
            fetchData.name = fetchObj.value("name").toString();
            fetchData.rowCount = fetchObj.value("rowCount").toInt();
            fetchData.pageCount = fetchObj.value("pageCount").toInt();
            fetchData.deleteCount = fetchObj.value("deleteCount").toInt();
            m_fetchData.tables.insert(fetchData);
        }
        // Start pulling
        pullNext();
    }
    else if (repQuery == "pull")
    {
        // If sync is not successful, stop syncing.
        if (!repSuccess)
        {
            m_lastError = tr("The server sync (pull) was not successful.\n"
                             "This is probably a bug or a configuration error.\n"
                             "Please report bugs on %1").arg(URL_SOURCECODE);
            qWarning().noquote() << "{Client}" << m_lastError;
            return;
        }

        // Store new data
        QJsonArray rowsArray = content.value("rows").toArray();
        QJsonArray deletedArray = content.value("deleted").toArray();
        QString table = content.value("table").toString();
        QSet<TableRow> rows;
        QStringList deletedUuids;

        if (m_pulledData.tables.contains(table))
            rows = m_pulledData.tables.value(table);

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
            row.role = rowObj.value("role").toString();
            rows.insert(row);
        }
        for (int i = 0; i < deletedArray.count(); i++)
        {
            QString uuid = deletedArray.at(i).toString();
            deletedUuids << uuid;
        }
        m_pulledData.tables.insert(table, rows);
        m_pulledData.deletedUuids.insert(table, deletedUuids);
        // Next pull
        pullNext();
    }
    else if (repQuery == "setPassword")
    {
        if (!repSuccess)
        {
            m_lastError = tr("Failed to set new password");
            qWarning().noquote() << "{Client}" << m_lastError;
        }
        else
        {
            qDebug().noquote() << "{Client}" << "The new password has correctly been set.";
        }
    }
}

void RamServerClient::sslError(QNetworkReply *reply, QList<QSslError> errs)
{
    Q_UNUSED(reply);

    foreach (QSslError err, errs)
    {
        qWarning() << "{Client}" << err.errorString();
    }
    m_lastError = tr("An SSL Error has occured. The connection may not be secured.");
    qWarning() << "{Client}" << m_lastError;
    setStatus(Offline);
}

void RamServerClient::flushRequests()
{
    if ( m_requestQueue.isEmpty() ) return;

    qInfo().noquote() << tr("Flushing remaining requests.");

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

    qInfo().noquote() << tr("All requests sent.");
}

void RamServerClient::networkError(QNetworkReply::NetworkError err)
{
    Q_UNUSED(err)

    QString reason = tr("Unknown network error.");
    auto reply = qobject_cast<QNetworkReply*>(sender());
    if (reply)
        reason = reply->errorString();

    qWarning() << "{Client}" << "A network error has occured: " + reason;
    setStatus(Offline);
    m_lastError = reason;
}

void RamServerClient::startQueue()
{
    if (m_status == ClientStatus::Offline) return;
    if (!m_requestQueueTimer->isActive()) m_requestQueueTimer->start(m_requestDelay);
}

void RamServerClient::pauseQueue()
{
    m_requestQueueTimer->stop();
}

void RamServerClient::queueRequest(QString query, QJsonObject body)
{
    Request r = buildRequest(query, body);
    queueRequest(r);
    startQueue();
}

void RamServerClient::queueRequest(Request r)
{
    m_requestQueue << r;
    startQueue();
}

Request RamServerClient::buildRequest(QString query, QJsonObject body)
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

Request RamServerClient::buildRequest(QString query)
{
    // Get info to build the URL
    QString address = serverAddress();
    QString protocol = serverProtocol();
    int port = serverPort();

    QUrl url(protocol + address + "?" + query);
    url.setPort(port);

    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));

    Request r;
    r.request = request;
    r.query = query;
    return r;
}

void RamServerClient::postRequest(Request r)
{
    QUrl url = r.request.url();
    QString host = url.host();

    // Verify if host is available
    QHostInfo info = QHostInfo::fromName(host);
    switch (info.error()) {
    case QHostInfo::HostNotFound:
    case QHostInfo::UnknownError:
        setStatus(Offline);
        qWarning().noquote() << "{Client}" << tr("The Ramses Server at '%1' is unreachable, sorry. Check your network connection.\n"
                                                 "We're switching to offline mode.").arg(host);
        return;
    case QHostInfo::NoError:
        break;
    }

    QNetworkReply *reply = m_network->post(r.request, r.body.toUtf8());

    // Log URL / GET
    qDebug().noquote() << "{Client}" << "New request: " +  url.toString(QUrl::RemovePassword);

// Log POST body (in debug mode only!)
#ifdef QT_DEBUG
    qDebug() << "Request data: " + r.body;
#endif

    connect(reply, &QNetworkReply::errorOccurred, this, &RamServerClient::networkError);
    connect(reply, &QNetworkReply::finished, reply, &QNetworkReply::deleteLater);
}

QNetworkReply *RamServerClient::synchronousRequest(Request r)
{
    // Log URL / GET
    QUrl url = r.request.url();
    QString host = url.host();

    // Verify if host is available
    QHostInfo info = QHostInfo::fromName(host);
    switch (info.error()) {
    case QHostInfo::HostNotFound:
    case QHostInfo::UnknownError:
        setStatus(Offline);
        qWarning().noquote() << "{Client}" << tr("The Ramses Server at '%1' is unreachable, sorry. Check your network connection.\n"
                                                 "We're switching to offline mode.").arg(host);
        m_lastError = tr("Server unavailable. Please check the server settings and your network connection.");
        return nullptr;
    case QHostInfo::NoError:
        break;
    }

    // Log URL / GET
    qDebug().noquote() << "{Client}" << "New request: " +  url.toString(QUrl::RemovePassword);

    // Log POST body (in debug mode only!)
#ifdef QT_DEBUG
    if (r.query == "login")
        qDebug() << "Request data: [Hidden login info]";
    else
        qDebug() << "Request data: " + r.body;
#endif

    // Create a loop to wait for the data
    QTimer timer;
    timer.setSingleShot(true);

    QNetworkReply *reply = m_synchronousNetwork->post(r.request, r.body.toUtf8());
    connect(reply, &QNetworkReply::errorOccurred, this, &RamServerClient::networkError);

    timer.start( m_timeout );

    while ( reply->isRunning() )
    {
        if (!timer.isActive()) {
            reply->abort();
            reply->deleteLater();
            setStatus(Offline);
            qWarning().noquote() << "{Client}" << tr("Time out: the Ramses Server at '%1' took too long to respond, sorry. Check your network connection.\n"
                                                     "We're switching to offline mode.").arg(host);
            m_lastError = tr("Server unavailable. Please check the server settings and your network connection.");
            return nullptr;
        }
        qApp->processEvents();
    }

    return reply;
}

QJsonObject RamServerClient::parseData(QNetworkReply *reply)
{
    if (!reply)
    {
        QJsonObject obj;
        obj.insert("success", false);
        obj.insert("message", "Connection failed, server unavailable or misconfigured.");
        return obj;
    }

    QString repAll = QString::fromUtf8(reply->readAll());
    reply->deleteLater();

    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    QJsonObject repObj = repDoc.object();

    if (repObj.isEmpty())
    {
        m_lastError = "The server returned invalid data.\n"
                      "This may be a misconfiguration of the server, a temporary network failure, or a bug.";
        repObj.insert("message",m_lastError);
        repObj.insert("accepted",false);
        repObj.insert("success",false);
        repObj.insert("query", "unknown");

        qDebug() << "{Client}" << repAll;

        return repObj;
    }

    QString repMessage = repObj.value("message").toString();
    bool repSuccess = repObj.value("success").toBool();

    if (!repSuccess) {
        m_lastError = repObj.value("message").toString("Unknown error");
        qWarning().noquote() << "{Client}" << m_lastError;
    }
    else if (repMessage.startsWith("warning", Qt::CaseInsensitive))
        qWarning().noquote() << "{Client}" << repMessage;
    else if (repMessage.startsWith("critical", Qt::CaseInsensitive))
        qCritical().noquote() << "{Client}" << repMessage;
    else
        qInfo().noquote() << "{Client}" << repMessage;

    return repObj;
}

void RamServerClient::startSync()
{
    qDebug().noquote() << "{Client}" << "Starting Sync...";

    setStatus(Syncing);

    queueRequest("sync");

    m_pulledData.syncDate = QDateTime::currentDateTimeUtc().toString(DATETIME_DATA_FORMAT);
    m_pulledData.tables = QHash<QString, QSet<TableRow>>();
}

void RamServerClient::fetch()
{
    qDebug().noquote() << "{Client}" << "Server Interface: Fetching changes...";

    queueRequest("fetch");
}

void RamServerClient::push(QString table, QSet<TableRow> rows, QString date, bool commit)
{
    qDebug().noquote() << "{Client}" << "Pushing" << rows.count() << "rows to" << table;

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
        rowsArray.append(rowObj);
    }
    body.insert("rows", rowsArray);
    body.insert("previousSyncDate", date);
    body.insert("commit", commit);
    queueRequest("push", body);
}

void RamServerClient::pull(QString table, int page)
{
    qDebug().noquote() << "{Client}" << "Pulling page #" << page << " from " << table;

    QJsonObject body;
    body.insert("table", table);
    body.insert("page", page);
    queueRequest("pull", body);
}

void RamServerClient::pushNext()
{
    // If the tables list is empty, we've pushed everything, commit
    if (m_pushingData.tables.isEmpty())
    {
        commit();
        return;
    }

    // Get a table and push its first 100 rows
    QString table = *m_pushingData.tables.keyBegin();
    QSet<TableRow> rows = m_pushingData.tables.value(table);
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
    m_pushingData.tables[table] = rows;

    // If we've got all rows, remove the table from the data to sync
    if (rows.isEmpty())
    {
        qInfo().noquote() << "{Client}" << "Uploading data to the server...";
        m_pushingData.tables.remove(table);
    }

    // Push the rows
    push(table, pushRows, m_pushingData.syncDate, false);
}

void RamServerClient::pullNext()
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

        qInfo().noquote() << "{Client}" << tr("Downloading new data from the server...");

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

void RamServerClient::commit()
{
    qInfo().noquote() << "{Client}" << "Commit!";

    push("", QSet<TableRow>(), "1818-05-05 00:00:00", true);
}

void RamServerClient::finishSync(bool inError)
{
    qDebug() << "{Client}" << "Finishing sync...";

    // Emit result
    if (!inError)
        emit syncReady( m_pulledData );

    if (m_status == Syncing)
        setStatus(Ready);

    // Make room
    m_pulledData = SyncData();

    if (!inError)
        qDebug().noquote() << "{Client}" << "Sync finished!";
    else
        qWarning().noquote() << "{Client}" << "Sync error!";
}

QString RamServerClient::hashPassword(const QString &password)
{
    QString address = m_serverAddress;
    address.replace("/", "");
    return DataCrypto::instance()->generatePassHash(password, address);
}

void RamServerClient::setStatus(ClientStatus status)
{
    if (m_status == status) return;

    switch(status) {
    case Offline:
        m_sessionToken = "";
        m_pulledData = SyncData();
        m_pushingData = SyncData();
        m_fetchData = FetchData();
        m_requestQueue.clear();
        qInfo().noquote() << "{Client}" << "Disconnected.";
        break;
    case Online:
        if (!m_ssl)
            qWarning().noquote() << "{Client}" << tr("The connection is not secured!");
        qInfo().noquote() << "{Client}" << tr("Connected!");
        break;
    case Ready:
        if(m_status == Online)
            qInfo().noquote() << "{Client}" << tr("Logged in!");
        else if (m_status == Syncing)
            qInfo().noquote() << "{Client}" << tr("Sync finished.");
        else
            qInfo().noquote() << "{Client}" << tr("Ready.");
        break;
    case Syncing:
        qInfo().noquote() << "{Client}" << tr("Synchronization...");
        break;
    }

    m_status = status;
    emit statusChanged(m_status);
}

RamServerClient::RamServerClient()
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

    connect(m_requestQueueTimer, &QTimer::timeout, this, &RamServerClient::nextRequest);
    connect(m_network, &QNetworkAccessManager::finished, this, &RamServerClient::dataReceived);
    connect(m_network, &QNetworkAccessManager::sslErrors, this, &RamServerClient::sslError);
    connect(qApp, &QApplication::aboutToQuit, this, &RamServerClient::flushRequests);
}
