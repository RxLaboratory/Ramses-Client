#include "dbinterface.h"

#include <QInputDialog>

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
    /*// Get the data to delete,
    // and clean the server data
    QHash<QString, QSet<QString> > deletedUuids = m_ldi->deletedUuids();
    m_rsi->deleteData( deletedUuids );

    resumeSync();
    // Full sync
    fullSync();*/
}

bool DBInterface::sync()
{
    switch (RamServerClient::i()->status()){
    case RamServerClient::Offline:
        m_lastError = tr("Can't sync for now, the client is offline.\nYou need to log in first.");
        qWarning().noquote() << "{DB}" << m_lastError;
        return false;
    case RamServerClient::Online:
        m_lastError = tr("Can't sync for now, we're logged out.\nYou need to log in first.");
        qWarning().noquote() << "{DB}" << m_lastError;
        return false;
    case RamServerClient::Ready:
        break;
    case RamServerClient::Syncing:
        qInfo().noquote() << "{DB}" << tr("Already syncing, please be patient");
        return true;
      break;
    }

    emit syncStarted();

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(3);
    pm->setText(tr("Beginning quick data sync..."));

    qInfo().noquote() << "{DB}" << tr("Beginning quick data sync...");

    // Get modified rows from local
    SyncData syncData = m_ldi->getSync( false );

    qInfo().noquote() << "{DB}" << tr("Pushing changes to the server...");

    // Post to ramserver
    RamServerClient::i()->sync(syncData);

    return true;
}

bool DBInterface::fullSync()
{
    switch (RamServerClient::i()->status()){
    case RamServerClient::Offline:
        m_lastError = tr("Can't sync for now, the client is offline.\nYou need to log in first.");
        qWarning().noquote() << "{DB}" << m_lastError;
        return false;
    case RamServerClient::Online:
        m_lastError = tr("Can't sync for now, we're logged out.\nYou need to log in first.");
        qWarning().noquote() << "{DB}" << m_lastError;
        return false;
    case RamServerClient::Ready:
        break;
    case RamServerClient::Syncing:
        qInfo().noquote() << "{DB}" << tr("Already syncing, please be patient");
        return true;
        break;
    }

    emit syncStarted();

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(3);
    pm->setText(tr("Beginning full data sync..."));
    qInfo().noquote() << "{DB}" << tr("Beginning full data sync...");

    // Get modified rows from local
    SyncData syncData = m_ldi->getSync( true );
    qInfo().noquote() << "{DB}" << tr("Pushing changes to the server...");

    // Post to ramserver
    RamServerClient::i()->sync(syncData);

    return true;
}

DBInterface::DBInterface(QObject *parent) : DuQFLoggerObject("Database Interface", parent)
{
    // LOCAL
    m_ldi = LocalDataInterface::instance();

    m_updateTimer = new QTimer(this);
    m_updateTimer->setSingleShot(true);

    connectEvents();
}

void DBInterface::connectEvents()
{
    connect(RamServerClient::i(), &RamServerClient::syncReady, m_ldi, QOverload<SyncData>::of(&LocalDataInterface::sync));
    connect(m_ldi, &LocalDataInterface::syncFinished, this, &DBInterface::finishSync);
    connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(sync()));

    connect(RamServerClient::i(), &RamServerClient::statusChanged,
            this, [this] (RamServerClient::ClientStatus status){
        if (m_syncSuspended)
            return;
        switch (status) {
        case RamServerClient::Offline:
        case RamServerClient::Online:
            suspendSync();
            return;
        case RamServerClient::Ready:
            resumeSync();
            return;
        case RamServerClient::Syncing:
            break;
        }
    } );
}

bool DBInterface::isSyncSuspended()
{
    return m_syncSuspended;
}

bool DBInterface::isTeamProject()
{
    auto config = m_ldi->serverConfig();
    return config.address != "";
}

void DBInterface::finishSync()
{
    emit syncFinished();
    m_updateTimer->start( m_updateFrequency );

    log(tr("Finished sync."));
}
