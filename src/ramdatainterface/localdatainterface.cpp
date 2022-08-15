#include "localdatainterface.h"

#include "datacrypto.h"

LocalDataInterface *LocalDataInterface::_instance = nullptr;

LocalDataInterface *LocalDataInterface::instance()
{
    if (!_instance) _instance = new LocalDataInterface();
    return _instance;
}

QString LocalDataInterface::login(QString username, QString password)
{
    // Get user data
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("SELECT data, uuid FROM RamUser WHERE userName = :userName;");
    query.bindValue(":userName", username);

    query.exec();

    if(!query.first()) return "";

    // Decrypt and check password
    QString data = query.value(0).toString();
    data = DataCrypto::instance()->clientDecrypt(data);

    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject dataObj = doc.object();

    QString test = dataObj.value("password").toString();
    if( password == test ) return query.value(1).toString();

    return "";
}

void LocalDataInterface::setRamsesPath(QString p)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    // Keep history
    query.prepare("UPDATE Settings SET current = 0 WHERE current = 1;");
    query.exec();

    // Add new
    query.prepare("INSERT INTO Settings (localDataPath, current) VALUES  (:path, 1);");
    query.bindValue(":path", p);

    query.exec();
}

QStringList LocalDataInterface::tableData(QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("SELECT uuid FROM :table WHERE removed = 0;");
    query.bindValue(":table", table);

    query.exec();

    QStringList data;

    while (query.next()) data << query.value(0).toString();

    return data;
}

bool LocalDataInterface::createObject(QString uuid, QString table, QString data)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    if (!query.exec("INSERT INTO '" % table % "' (uuid, data, modified, removed)"
                       "VALUES ( '" % uuid % "' , '" % data % "' , '" % modified.toString("yyyy-MM-dd hh:mm:ss:zzz") % "' , 0);"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + query.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + query.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + query.lastError().driverText();
        log(errorMessage, DuQFLog::Critical);
        return false;
    }

    return true;
}

QString LocalDataInterface::objectData(QString uuid, QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("SELECT data FROM :table WHERE uuid = :uuid;");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();

    if (query.first()) return query.value(0).toString();
    return "";
}

void LocalDataInterface::setObjectData(QString uuid, QString table, QString data)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("UPDATE :table SET data = :data, modified = :modified WHERE uuid = :uuid;");
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
    query.prepare("UPDATE :table SET removed = 0 WHERE uuid = :uuid;");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();
}

void LocalDataInterface::restoreObject(QString uuid, QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    // Set removed to 1
    query.prepare("UPDATE :table SET removed = 1 WHERE uuid = :uuid;");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();
}

bool LocalDataInterface::isRemoved(QString uuid, QString table)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("SELECT removed FROM :table WHERE uuid = :uuid;");
    query.bindValue(":table", table);
    query.bindValue(":uuid", uuid);

    query.exec();


    if (query.first())
    {
        return query.value(0).toBool();
    }

    return true;
}

void LocalDataInterface::setUsername(QString uuid, QString username)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = QSqlQuery(db);

    query.prepare("UPDATE RamUser SET userName = :userName WHERE uuid = :uuid;");
    query.bindValue(":userName", username);
    query.bindValue(":uuid", uuid);

    query.exec();
}

ServerConfig LocalDataInterface::serverConfig() const
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery query = db.exec("SELECT address, useSsl, updateDelay, timeout FROM RamServer;");

    ServerConfig config;
    if (query.first()) {
        config.address = query.value(0).toString();
        config.useSsl = query.value(1).toBool();
        config.updateDelay = query.value(2).toInt();
        config.timeout = query.value(3).toInt();
    }

    return config;
}

const QString &LocalDataInterface::dataFile() const
{
    return m_dataFile;
}

ServerConfig LocalDataInterface::setDataFile(const QString &file)
{
    QSqlDatabase db = QSqlDatabase::database("localdata");
    // Set the SQLite file
    db.close();
    // Open
    db.setDatabaseName(file);
    if (!db.open()) log("Can't save data to the disk.", DuQFLog::Fatal);

    return serverConfig();
}

LocalDataInterface::LocalDataInterface()
{
    //Load local database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","localdata");
    db.setHostName("localhost");
}
