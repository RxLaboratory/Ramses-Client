#include "dbinterface.h"

DBInterface *DBInterface::_instance = nullptr;

DBInterface *DBInterface::instance()
{
    if (!_instance) _instance = new DBInterface();
    return _instance;
}

void DBInterface::setOffline()
{
    // Disconnects from the Ramses Server and change connection status
    m_rsi->setOffline();
    setConnectionStatus(NetworkUtils::Offline, "Disconnecting from the Ramses Server.");
}

void DBInterface::setOnline()
{
    // Connects to the Ramses Server and change connection status
    m_rsi->setOnline();
    setConnectionStatus(NetworkUtils::Connecting, "Connecting to the Ramses Server...");
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

    return uuid;
}

QStringList DBInterface::tableData(QString table)
{
    return m_ldi->tableData(table);
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
    }
    return config;
}

DBInterface::DBInterface(QObject *parent) : DuQFLoggerObject("Database Interface", parent)
{
    // LOCAL
    m_ldi = LocalDataInterface::instance();
    // REMOTE
    m_rsi = RamServerInterface::instance();
}

void DBInterface::connectEvents()
{
    connect(m_rsi, SIGNAL(connectionStatusChanged(NetworkUtils::NetworkStatus)), this, SLOT(serverConnectionStatusChanged(NetworkUtils::NetworkStatus)));
}

NetworkUtils::NetworkStatus DBInterface::connectionStatus() const
{
    return m_connectionStatus;
}

void DBInterface::setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason)
{
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
