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
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("SELECT data FROM :table WHERE uuid = :uuid");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();

    if (query.first()) {
        QString dataStr = query.value(0).toString();
        QJsonDocument doc = QJsonDocument::fromJson(dataStr.toUtf8());
        return doc.object();
    }

    return QJsonObject();
}

void LocalDataInterface::setObjectData(QString uuid, QString table, QJsonObject data)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("UPDATE :table SET data = :data, modified = :modified WHERE uuid = :uuid");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);
    query.bindValue(":data", data);
    query.bindValue(":modified", modified.toString("yyy-MM-dd hh:mm:ss:zzz"));

    query.exec();
}

void LocalDataInterface::removeObject(QString uuid, QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    // Set removed to 0
    query.prepare("UPDATE :table SET removed = 0 WHERE uuid = :uuid");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();
}

void LocalDataInterface::restoreObject(QString uuid, QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    // Set removed to 1
    query.prepare("UPDATE :table SET removed = 1 WHERE uuid = :uuid");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);
}

ServerConfig LocalDataInterface::serverConfig()
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = db.exec("SELECT address, useSsl, updateDelay, timeout, path FROM RamServer;");

    ServerConfig config;
    if (query.first()) {
        config.address = query.value(0).toString();
        config.useSsl = query.value(1).toBool();
        config.updateDelay = query.value(2).toInt();
        config.timeout = query.value(3).toInt();
        config.path = query.value(4).toString();
    }

    return config;
}

const QString &LocalDataInterface::dataFile() const
{
    return m_dataFile;
}

void LocalDataInterface::setDataFile(const QString &file)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    // Set the SQLite file
    db.close();
    // Open
    db.setDatabaseName(file);
    if (!db.open()) log("Can't save data to the disk.", DuQFLog::Fatal);
}

LocalDataInterface::LocalDataInterface()
{
    //Load local database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","localdata");
    db.setHostName("localhost");
}
