#include "dbinterface.h"

#include <QInputDialog>

#include "duutils/guiutils.h"
#include "progressmanager.h"
#include "ramserverclient.h"
#include "statemanager.h"
#include "ramsettings.h"

DBInterface *DBInterface::_instance = nullptr;

QStringList DBInterface::recentDatabases()
{
    QStringList projects;

    const QVector<QHash<QString,QVariant>> recentList = DuSettings::i()->getArray(RamSettings::RecentDatabases);
    for(const auto &recent: recentList) {
        QString p = recent.value("path", "").toString();
        if (QFileInfo::exists(p))
            projects << p;
    }

    return projects;
}

void DBInterface::addToRecentList(const QString &dbFile)
{
    QFileInfo dbInfo(dbFile);

    QVector<QHash<QString,QVariant>> recentList;

    QHash<QString,QVariant> recentDB = {{ "path", dbFile }};
    recentList << recentDB;

    const QStringList &savedRecent = recentDatabases();
    for (const auto &p: savedRecent) {
        if (  dbInfo == QFileInfo(p) )
            continue;
        QHash<QString,QVariant> recentDB = {{ "path", p }};
        recentList << recentDB;
    }

    DuSettings::i()->setArray(RamSettings::RecentDatabases, recentList);
}

bool DBInterface::isTeamProject(const QString &dbFile)
{
    auto config = LocalDataInterface::getServerSettings(dbFile);
    return config.address != "";
}

DBInterface *DBInterface::i()
{
    if (!_instance) _instance = new DBInterface();
    return _instance;
}

void DBInterface::setOffline(bool sync)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setText( tr("Disconnecting from the Ramses Server...") );
    pm->addToMaximum(2);

    m_disconnecting = true;

    // One last sync
    if (sync && m_rsi->isOnline() && !m_syncSuspended)
    {
        pm->setText( tr("One last sync!") );
        pm->increment();

        fullSync();
        suspendAutoSync();
        return;
    }

    pm->setText( tr("Disconnecting.") );
    pm->increment();
    finishSync();
}

void DBInterface::setOnline(QString serverUuid)
{
    if (m_ldi->dataFile() == "")
    {
        QMessageBox::information(
                    GuiUtils::appMainWindow(),
                    tr("No database"),
                    tr("There's no open database.\n\n"
                       "I'm sorry, you need to open or create a database to get online.")
                    );
        return;
    }

    // Connects to the Ramses Server and change connection status
    m_rsi->setOnline(serverUuid);
}

void DBInterface::setRamsesPath(QString p)
{
    m_ldi->setRamsesPath(p);
}

QString DBInterface::ramsesPath()
{
    return m_ldi->ramsesPath();
}

QSet<QString> DBInterface::tableUuids(QString table)
{
    return m_ldi->tableUuids(table);
}

bool DBInterface::contains(QString uuid, QString table, bool includeRemoved)
{
    return m_ldi->contains(uuid, table, includeRemoved);
}

void DBInterface::createObject(QString uuid, QString table, QString data)
{
    m_ldi->createObject(uuid, table, data);
}

QString DBInterface::objectData(QString uuid, QString table)
{
    return m_ldi->objectData(uuid, table);
}

void DBInterface::setObjectData(QString uuid, QString table, QString data)
{
    m_ldi->setObjectData(uuid, table, data);
}

QString DBInterface::validateObjectData(QString data, QString uuid, QString type, bool ignoreErrors)
{
    if (data == "") return "{}";
    // Try to create a JSON Doc
    QJsonParseError *e = new QJsonParseError();
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8(), e);

    // OK, return a compact json
    if (e->error == QJsonParseError::NoError) return doc.toJson(QJsonDocument::Compact);

    if (ignoreErrors) return "";

    // Try some fixes
    QString testData = data;

    // A common error lies with new lines. Try to fix.
    // Remove carriage returns, to accept only new lines
    testData.replace("\r", "");
    // Try escaping new lines
    testData.replace("\n","\\n");
    // Test again
    testData = validateObjectData(testData, uuid, type, true);
    // It worked
    if (testData != "") return testData;

    // Another attempt at fixing new lines
    testData = data;
    // Remove carriage returns, to accept only new lines
    testData.replace("\r", "");
    // Try removing new lines
    testData.replace("\n","");
    // Test again
    testData = validateObjectData(testData, uuid, type, true);
    // It worked
    if (testData != "") return testData;

    // Everything failed

    QString eStr = "";
    if (uuid != "") eStr = "Object with uuid: " + uuid + " contains invalid data.\n";
    else eStr = "An unknown object contains invalid data.\n";
    eStr += "This data will be removed, sorry.";
    if (type != "") eStr += "Object type: " + type + "\n";
    eStr += "Parse error: " + e->errorString() + "\n";
    eStr += "Original data:\n" + data;

    // Log the error
    log(eStr, DuQFLog::Warning);

    return "{}";
}

void DBInterface::removeObject(QString uuid, QString table)
{
    m_ldi->removeObject(uuid, table);
}

void DBInterface::restoreObject(QString uuid, QString table)
{
    m_ldi->restoreObject(uuid, table);
}

bool DBInterface::isRemoved(QString uuid, QString table)
{
    return m_ldi->isRemoved(uuid, table);
}

QString DBInterface::modificationDate(QString uuid, QString table)
{
    return m_ldi->modificationDate(uuid, table);
}

const QString &DBInterface::dataFile() const
{
    return m_ldi->dataFile();
}

bool DBInterface::loadDataFile(const QString &file)
{
    if (!QFileInfo::exists(file)) {
        m_lastError = "Invalid path: " + file;
        qWarning().noquote() << "{DB}" << m_lastError;
        return false;
    }

    // If there is a current project, just restart the app passing the new project as argument.
    if (m_ldi->dataFile() != "") {
        qInfo().noquote() << "{DB}" << "Restarting the app to load \"" + file + "\"" ;
        StateManager::i()->restart(true, file);
        return true;
    }

    QElapsedTimer openTimer;
    openTimer.start();

    qInfo().noquote() << "{DB}" << "Loading database \"" + file + "\"" ;

    ServerConfig config = m_ldi->setDataFile(file);

    qDebug().noquote() << "{DB}" << "Database loaded: " << openTimer.elapsed()/1000 << " seconds.";

    // TEAM / ONLINE
    if (config.address != "")
    {
        // Update server settings

        m_rsi->setServerAddress(config.address);
        m_rsi->setTimeout(config.timeout);
        m_rsi->setSsl(config.useSsl);
        m_rsi->setServerPort(config.port);
        m_updateFrequency = config.updateDelay;
    }
    else
    {
        m_rsi->setServerAddress("");
    }

    return true;
}

QString DBInterface::getUserRole(const QString &uuid)
{
    return m_ldi->getUserRole(uuid);
}

bool DBInterface::setUserRole(const QString &uuid, const QString role)
{
    QJsonObject obj = RamServerClient::i()->setUserRole(uuid, role);
    if (!obj.value("success").toBool(false))
        return false;
    m_ldi->setUserRole(uuid, role);
    return true;
}

void DBInterface::suspendSync()
{
    m_updateTimer->stop();
    m_syncSuspended = true;
}

void DBInterface::resumeSync()
{
    m_syncSuspended = false;
    m_updateTimer->start(m_updateFrequency);
}

void DBInterface::suspendAutoSync()
{
    m_autoSyncSuspended = true;
    m_updateTimer->stop();
}

void DBInterface::resumeAutoSync()
{
    m_autoSyncSuspended = false;
    m_updateTimer->start(m_updateFrequency);
}

QString DBInterface::cleanDabaBase(int deleteDataOlderThan)
{
    qDebug() << ">>> Beginning database clean...";

    // Stop auto sync
    qDebug() << "Suspending sync!";
    suspendSync();

    // Clean !
    qDebug() << "--- Cleaning ---";
    return m_ldi->cleanDataBase( deleteDataOlderThan );
}

bool DBInterface::undoClean()
{
    bool ok = m_ldi->undoClean();
    if (!ok) {
        log(tr("Can't restore DB Backup!"), DuQFLog::Critical);
        return false;
    }

    StateManager::i()->restart(false);
    return true;
}

void DBInterface::acceptClean()
{
    // Get the data to delete,
    // and clean the server data
    QHash<QString, QSet<QString> > deletedUuids = m_ldi->deletedUuids();
    m_rsi->deleteData( deletedUuids );

    resumeSync();
    // Full sync
    fullSync();
}

void DBInterface::sync()
{
    if (m_syncSuspended) {
        log(tr("Sync is suspended!"), DuQFLog::Warning);
        return;
    }
    if (m_connectionStatus != NetworkUtils::Online) return;

    emit syncStarted();

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(3);
    pm->setText(tr("Beginning quick data sync..."));

    // Get modified rows from local
    SyncData syncData = m_ldi->getSync( false );

    log(tr("Pushing changes to the server..."));
    // Post to ramserver
    m_rsi->sync(syncData);
}

void DBInterface::fullSync()
{
    if (m_syncSuspended) {
        log(tr("Sync is suspended!"), DuQFLog::Warning);
        return;
    }
    if (m_connectionStatus != NetworkUtils::Online) return;

    emit syncStarted();

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(3);
    pm->setText(tr("Beginning full data sync..."));

    // Get modified rows from local
    SyncData syncData = m_ldi->getSync( true );
    log(tr("Pushing data to the server..."));
    // Post to ramserver
    m_rsi->sync(syncData);
}

DBInterface::DBInterface(QObject *parent) : DuQFLoggerObject("Database Interface", parent)
{
    // LOCAL
    m_ldi = LocalDataInterface::instance();
    // REMOTE
    m_rsi = RamServerInterface::instance();

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);

    connectEvents();
}

void DBInterface::connectEvents()
{
    connect(RamServerClient::i(), &RamServerClient::syncReady, m_ldi, QOverload<SyncData>::of(&LocalDataInterface::sync));

    connect(m_ldi, &LocalDataInterface::syncFinished, this, &DBInterface::finishSync);

    // Deprecated
    connect(m_rsi, &RamServerInterface::connectionStatusChanged, this, &DBInterface::serverConnectionStatusChanged);
    connect(m_rsi, &RamServerInterface::syncReady, m_ldi, QOverload<SyncData,QString>::of(&LocalDataInterface::sync));
    connect(m_rsi, &RamServerInterface::pong, m_ldi, &LocalDataInterface::setServerUuid);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(sync()));
}

NetworkUtils::NetworkStatus DBInterface::connectionStatus() const
{
    return m_connectionStatus;
}

bool DBInterface::isSyncSuspended()
{
    return m_syncSuspended;
}

bool DBInterface::isAutoSyncSuspended()
{
    return m_autoSyncSuspended;
}

bool DBInterface::isTeamProject()
{
    auto config = m_ldi->serverConfig();
    return config.address != "";
}

void DBInterface::setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason)
{
    if (s == m_connectionStatus) return;
    m_connectionStatus = s;
    emit connectionStatusChanged(s, reason);
}

void DBInterface::finishSync()
{
    if (m_disconnecting)
    {
        // Disconnects from the Ramses Server
        m_rsi->setOffline();
    }

    emit syncFinished();
    if (!m_autoSyncSuspended) m_updateTimer->start( m_updateFrequency );
    log(tr("Finished sync."));
}

void DBInterface::serverConnectionStatusChanged(NetworkUtils::NetworkStatus status)
{
    switch(status)
    {
    case NetworkUtils::Offline:
        setConnectionStatus(status, "Disconnected from the Ramses Server.");
        suspendSync();
        suspendAutoSync();
        break;
    case NetworkUtils::Online:
        setConnectionStatus(status, "Connected to the Ramses Server.");
        resumeSync();
        resumeAutoSync();
        break;
    default:
        return;
    }
}
