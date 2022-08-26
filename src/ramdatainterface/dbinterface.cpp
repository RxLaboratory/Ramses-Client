﻿#include "dbinterface.h"

DBInterface *DBInterface::_instance = nullptr;

DBInterface *DBInterface::instance()
{
    if (!_instance) _instance = new DBInterface();
    return _instance;
}

bool DBInterface::isReady() const
{
    return m_ldi->isReady();
}

void DBInterface::setOffline(QString reason)
{
    // One last sync
    if (m_rsi->isOnline()) sync();
    // Wait for server timeout to be able to sync
    QDeadlineTimer t( m_rsi->timeOut() );
    while (true)
    {
        qApp->processEvents();
        if (t.hasExpired()) break;
    }
    // Disconnects from the Ramses Server and change connection status
    m_rsi->setOffline();
    setConnectionStatus(NetworkUtils::Offline, reason);
    m_updateTimer->stop();
}

void DBInterface::setOnline()
{
    // Connects to the Ramses Server and change connection status
    m_rsi->setOnline();
    m_updateTimer->start(m_updateFrequency);
}

void DBInterface::setRamsesPath(QString p)
{
    m_ldi->setRamsesPath(p);
}

QString DBInterface::login(QString username, QString password)
{
    // 1- Check local data
    QString uuid = m_ldi->login(username, password);
    if ( uuid == "" ) return "";

    // 2- Check online server
    if (m_connectionStatus == NetworkUtils::Online)
    {
        // TODO
        // generate server passhash using m_serverAddress.toLower().replace("/",".") as prefix
        // m_rsi->login(uuid, password);
    }

    // First sync
    sync();

    return uuid;
}

QStringList DBInterface::tableData(QString table)
{
    return m_ldi->tableData(table);
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
}

const QString &DBInterface::dataFile() const
{
    return m_ldi->dataFile();
}

ServerConfig DBInterface::setDataFile(const QString &file)
{
    ServerConfig config = m_ldi->setDataFile(file);
    // Set the new server params
    if (config.address != "")
    {
        m_rsi->setServerAddress(config.address);
        m_rsi->setTimeout(config.timeout);
        m_rsi->setSsl(config.useSsl);
        m_updateFrequency = config.updateDelay*1000;
        setOnline();
    }
    else
    {
        setOffline(tr("Database set to offline mode"));
    }
    return config;
}

void DBInterface::sync()
{
    // Get modified rows from local
    QJsonObject syncBody = m_ldi->getSync();
    // Post to ramserver
    m_rsi->sync(syncBody);
}

void DBInterface::quit()
{
    setOffline("About to quit.");
}

DBInterface::DBInterface(QObject *parent) : DuQFLoggerObject("Database Interface", parent)
{
    // LOCAL
    m_ldi = LocalDataInterface::instance();
    // REMOTE
    m_rsi = RamServerInterface::instance();

    m_updateTimer = new QTimer(this);

    connectEvents();
}

void DBInterface::connectEvents()
{
    connect(m_ldi, &LocalDataInterface::dataReset, this, &DBInterface::dataReset);
    connect(m_rsi, &RamServerInterface::connectionStatusChanged, this, &DBInterface::serverConnectionStatusChanged);
    connect(m_rsi, &RamServerInterface::syncReady, m_ldi, &LocalDataInterface::sync);
    connect(m_updateTimer, &QTimer::timeout, this, &DBInterface::sync);

    connect(qApp, &QApplication::aboutToQuit, this, &DBInterface::quit);
}

NetworkUtils::NetworkStatus DBInterface::connectionStatus() const
{
    return m_connectionStatus;
}

void DBInterface::setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason)
{
    if (s == m_connectionStatus) return;
    m_connectionStatus = s;
    emit connectionStatusChanged(s, reason);
}

void DBInterface::serverConnectionStatusChanged(NetworkUtils::NetworkStatus status)
{
    switch(status)
    {
    case NetworkUtils::Offline:
        setConnectionStatus(status, "Disconnected from the Ramses Server.");
        break;
    case NetworkUtils::Online:
        setConnectionStatus(status, "Connected to the Ramses Server.");
        break;
    default:
        return;
    }
}
