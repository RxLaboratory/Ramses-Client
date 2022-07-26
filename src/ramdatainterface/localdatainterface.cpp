#include "localdatainterface.h"
#include "ramses.h"

LocalDataInterface LocalDataInterface::*_instance = nullptr;

LocalDataInterface *LocalDataInterface::instance()
{
    if (!_instance) _instance = new LocalDataInterface();
    return _instance;
}

QJsonObject LocalDataInterface::objectData(QString uuid, QString table)
{
    // TODO
    // Get the data in the SQLite DB

    return QJsonObject();
}

void LocalDataInterface::setObjectData(QString uuid, QString table, QJsonObject data)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();
    // TODO
    // Save to SQLite DB
}

void LocalDataInterface::removeObject(QString uuid, QString table)
{
    // TODO
    // Set removed to 0
}

void LocalDataInterface::restoreObject(QString uuid, QString table)
{
    // TODO
    // Set removed to 1
}

void LocalDataInterface::setServerAddress(QString address)
{
    // Set the SQLite file
    m_localDB.close();

    // Get the file
    QString fileName = address.trimmed().replace("/","") + ".ramdb";
    QString path = Ramses::instance()->pathFromRamses(RamObject::subFolderName(RamObject::DataFolder), true);
    QString dbPath = path + "/" + fileName;
    if (!QFileInfo::exists(dbPath)) {
        // Copy the template to use it
        FileUtils::copy(":/data/template", dbPath);
    }

    // Open
    m_localDB.setDatabaseName(dbPath);
    if (!m_localDB.open()) log("Can't save data to the disk.", DuQFLog::Fatal);
}

LocalDataInterface::LocalDataInterface()
{
    //Load local database
    m_localDB = QSqlDatabase::addDatabase("QSQLITE","localdata");
    m_localDB.setHostName("localhost");
}
