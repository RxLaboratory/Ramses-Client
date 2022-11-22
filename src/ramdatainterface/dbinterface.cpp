#include "dbinterface.h"
#include "duqf-utils/guiutils.h"
#include "progressmanager.h"

DBInterface *DBInterface::_instance = nullptr;

DBInterface *DBInterface::instance()
{
    if (!_instance) _instance = new DBInterface();
    return _instance;
}

void DBInterface::setOffline()
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setText( tr("Disconnecting from the Ramses Server...") );
    pm->addToMaximum(2);

    m_disconnecting = true;

    // One last sync
    if (m_rsi->isOnline() && !m_syncSuspended)
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

bool DBInterface::contains(QString uuid, QString table)
{
    return m_ldi->contains(uuid, table);
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

void DBInterface::setUsername(QString uuid, QString username)
{
    m_ldi->setUsername(uuid, username);
    // Setting the username must trigger an instant sync (if it's not new)
    if (username.toLower() != "new") sync();
}

bool DBInterface::isUserNameAavailable(const QString &userName)
{
    return m_ldi->isUserNameAavailable(userName);
}

const QString &DBInterface::dataFile() const
{
    return m_ldi->dataFile();
}

void DBInterface::setDataFile(const QString &file, bool ignoreUser)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setText(tr("Loading database..."));

    ServerConfig config = m_ldi->setDataFile(file);
    // Set the new server params
    if (config.address != "")
    {
        m_rsi->setServerAddress(config.address);
        m_rsi->setTimeout(config.timeout);
        m_rsi->setSsl(config.useSsl);
        m_rsi->setServerPort(config.port);
        m_updateFrequency = config.updateDelay;

        // Get the serverUuid we should be connecting to
        QString serverUuid = m_ldi->serverUuid();

        // Check the user
        QString userUuid = m_ldi->currentUserUuid();

        qDebug() << "Selected previous user: " << userUuid;

        emit userChanged( userUuid );
        setOnline(serverUuid);

        pm->setText(tr("Ready!"));
        pm->finish();
    }
    else
    {
        setOffline();
        m_rsi->setServerAddress("");
        if (file == "")
        {
            pm->setText(tr("Ready!"));
            pm->finish();
            return;
        }

        // Check the user
        QString userUuid = m_ldi->currentUserUuid();

        // Check if it exists in the database
        if (!m_ldi->contains(userUuid, "RamUser"))
        {
            userUuid = "";
        }

        if (userUuid != "")
        {
            emit userChanged( userUuid );
            pm->setText(tr("Ready!"));
            pm->finish();
            return;
        }

        if (ignoreUser)
        {
            emit userChanged( userUuid );
            pm->setText(tr("Ready!"));
            pm->finish();
            return;
        }

        // We need to show a list of users
        QVector<QStringList> users = m_ldi->users();
        QStringList names;
        for (int i = 0; i < users.count(); i++)
        {
            names << users[i][1];
        }
        bool ok;
        QString name = QInputDialog::getItem(
                    GuiUtils::appMainWindow(),
                    tr("Unknown user"),
                    tr("Who are you?"),
                    names,
                    0,
                    false,
                    &ok );

        if (ok && !name.isEmpty())
        {
            QString uuid;
            for (int i = 0; i < users.count(); i++)
            {
                if (users[i][1] == name)
                {
                    uuid = users[i][0];
                    break;
                }
            }
            if (uuid != "") setCurrentUserUuid(uuid);
            emit userChanged( userUuid );
            pm->setText(tr("Ready!"));
            pm->finish();
            return;
        }
    }
}

void DBInterface::setCurrentUserUuid(QString uuid)
{
    m_ldi->setCurrentUserUuid(uuid);
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

    AppUtils::restartApp();
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
    connect(m_ldi, &LocalDataInterface::syncFinished, this, &DBInterface::finishSync);
    connect(m_rsi, &RamServerInterface::connectionStatusChanged, this, &DBInterface::serverConnectionStatusChanged);
    connect(m_rsi, &RamServerInterface::syncReady, m_ldi, &LocalDataInterface::sync);
    connect(m_rsi, &RamServerInterface::userChanged, this, &DBInterface::serverUserChanged);
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

void DBInterface::setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason)
{
    if (s == m_connectionStatus) return;
    m_connectionStatus = s;
    emit connectionStatusChanged(s, reason);
}

void DBInterface::serverUserChanged(QString userUuid, QString username, QString data, QString modified)
{
    // Check if we should be online
    ServerConfig config = m_ldi->serverConfig();
    if (config.address != "") // online
    {
        if (userUuid != "") // OK
        {
            if (username != "" && data != "") m_ldi->updateUser(userUuid, username, data, modified);
            emit userChanged(userUuid);
            return;
        }

        // The ldi doesn't have user either: can't do anything
        if (m_ldi->currentUserUuid() == "")
        {
            QMessageBox::warning(
                        GuiUtils::appMainWindow(),
                        tr("Can't log in"),
                        tr("I'm sorry, I can't log you in.\n\n"
                           "I don't know who you are, and the authentification failed on the server.\n"
                           "You have to connect to the server before you can use this database.")
                        );
            emit userChanged("");
            return;
        }
    }
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
