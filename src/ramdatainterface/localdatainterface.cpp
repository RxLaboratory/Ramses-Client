#include "localdatainterface.h"

#include "datacrypto.h"
#include "duqf-utils/utils.h"
#include "processmanager.h"

// QUERIER

Querier::Querier(QString dbName): DuQFLoggerObject("Local Data Interface Querier")
{
    m_dbName = dbName;
}

void Querier::setDataFile(QString f)
{
    m_dataFile = f;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_dbName);
    db.setHostName("localhost");
    db.setDatabaseName(m_dataFile);
    if (!db.open())
    {
        emit error("Can't access data from the disk.");
    }
}

QSqlQuery Querier::query(QString q)
{
    if (m_dataFile =="")
    {
        emit queryFinished(q);
        return QSqlQuery();
    }

    //Load local database
    QSqlDatabase db = QSqlDatabase::database(m_dbName);
    QSqlQuery qry = QSqlQuery(db);

    //log(tr("Querying:") + "\n" + q, DuQFLog::Data);
    //qDebug() << q;

    if (!qry.exec(q))
    {
        QString errorMessage = "Something went wrong when %1 the data.\nHere's some information:";
        if (m_dbName == "getter") errorMessage = errorMessage.arg("getting");
        else errorMessage = errorMessage.arg("saving");
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        emit error(errorMessage);
        emit queryFinished(q);
        return QSqlQuery();
    }
    emit ready(qry);
    emit queryFinished(q);
    return qry;
}

// INTERFACE

LocalDataInterface *LocalDataInterface::_instance = nullptr;

LocalDataInterface *LocalDataInterface::instance()
{
    if (!_instance) _instance = new LocalDataInterface();
    return _instance;
}

bool LocalDataInterface::isReady() const
{
    return m_activeQueries.isEmpty();
}

void LocalDataInterface::waitForReady(int timeout) const
{
    QDeadlineTimer t(timeout);
    while (!LocalDataInterface::isReady())
    {
        qApp->processEvents();
        if (t.hasExpired()) return;
    }
    return;
}

void LocalDataInterface::setServerSettings(QString dbFile, ServerConfig c)
{
    // Make sure the interface is ready
    LocalDataInterface::instance();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    // Set the SQLite file
    db.close();
    // Open
    db.setDatabaseName(dbFile);
    if (!db.open()) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

    // Remove previous settings
    QSqlQuery qry = QSqlQuery(db);
    if (!qry.exec("DELETE FROM RamServer;"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);

        db.close();
        return;
    }

    // Add new settings
    QString q = "INSERT INTO RamServer (address, useSsl, updateDelay, timeout) "
            "VALUES ('%1', %2, %3, %4)";

    QString useSsl = "1";
    if (!c.useSsl) useSsl = "0";
    if (!qry.exec(q.arg(c.address, useSsl, QString::number(c.updateDelay), QString::number(c.timeout))))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
    }

    db.close();
}

ServerConfig LocalDataInterface::getServerSettings(QString dbFile)
{
    // Make sure the interface is ready
    LocalDataInterface::instance();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    // Set the SQLite file
    db.close();
    // Open
    db.setDatabaseName(dbFile);
    if (!db.open()) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

    // Get settings
    QSqlQuery qry = QSqlQuery(db);

    if (!qry.exec("SELECT address, useSsl, updateDelay, timeout FROM RamServer;"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);

        db.close();
        return ServerConfig();
    }

    ServerConfig s;

    if (qry.first())
    {
        s.address = qry.value(0).toString();
        s.useSsl = qry.value(1).toBool();
        s.updateDelay = qry.value(2).toInt();
        s.timeout = qry.value(3).toInt();
    }

    db.close();

    return s;
}

QString LocalDataInterface::login(QString username, QString password)
{
    username.replace("'", "''");

    // Get user data
    QString q = "SELECT data, uuid FROM RamUser WHERE userName = '%1';";
    QSqlQuery qry = query( q.arg(username) );

    if(!qry.first()) return "";

    // Decrypt and check password
    QString data = qry.value(0).toString();

    if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientDecrypt(data);

    QJsonDocument doc = QJsonDocument::fromJson( data.toUtf8() );
    QJsonObject dataObj = doc.object();

    QString test = dataObj.value("password").toString();

    if ( password == test )
    {
        // Save the current DB to recent files
        QSettings settings;
        // Get all to remove non existing, and insert new at first
        QStringList dbs;
        int n = settings.beginReadArray("database/recent");
        for (int i = 0; i < n; i++)
        {
            settings.setArrayIndex(i);
            dbs << settings.value("path", "-").toString();
        }
        settings.endArray();
        // Check
        for (int i = dbs.count() - 1; i >= 0; i--)
        {
            if (!QFileInfo::exists(dbs.at(i)))
            {
                dbs.removeAt(i);
                continue;
            }
            if (dbs.at(i) == m_dataFile) dbs.removeAt(i);
        }
        // Insert
        dbs.insert(0, m_dataFile);
        // Write
        settings.beginWriteArray("database/recent");
        for (int i = 0; i < dbs.count(); i++)
        {
            settings.setArrayIndex(i);
            settings.setValue("path", dbs.at(i));
        }
        settings.endArray();

        return qry.value(1).toString();
    }

    return "";
}

void LocalDataInterface::setRamsesPath(QString p)
{
    p.replace("'", "''");

    // Keep history
    QString q = "UPDATE Settings SET current = 0 WHERE current = 1;";
    query( q );

    // Add new
    q = "INSERT INTO Settings (localDataPath, current) VALUES  ( '%1', 1);";
    threadedQuery( q.arg(p) );

}

QStringList LocalDataInterface::tableData(QString table)
{
    QString q = "SELECT uuid FROM '%1' WHERE removed = 0;";
    QSqlQuery qry = query( q.arg(table) );

    QStringList data;

    while (qry.next()) data << qry.value(0).toString();

    return data;
}

bool LocalDataInterface::contains(QString uuid, QString table)
{
    QString q = "SELECT uuid FROM '%1' WHERE uuid = '%2' AND removed = 0;";
    q = q.arg(table, uuid);
    QSqlQuery qry = query( q );

    if (qry.first() && qry.value(0) != "") return true;
    return false;
}

void LocalDataInterface::createObject(QString uuid, QString table, QString data)
{
    data.replace("'", "''");

    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "INSERT INTO '%1' (uuid, data, modified, removed) "
                "VALUES ('%2', '%3', '%4', 0) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified ;";

    threadedQuery( q.arg(
                  table,
                  uuid,
                  data,
                  modified.toString("yyyy-MM-dd hh:mm:ss:zzz")
                  )
            );
}

QString LocalDataInterface::objectData(QString uuid, QString table)
{
    QString q = "SELECT data FROM %1 WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    if (qry.first()) return qry.value(0).toString();
    return "";
}

void LocalDataInterface::setObjectData(QString uuid, QString table, QString data)
{
    data.replace("'", "''");

    QDateTime modified = QDateTime::currentDateTimeUtc();


    QString q = "INSERT INTO %1 (data, modified, uuid) "
                "VALUES ( '%2', '%3', '%4') "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified ;";

    threadedQuery( q.arg(table, data, modified.toString("yyyy-MM-dd hh:mm:ss:zzz"), uuid) );
}

void LocalDataInterface::removeObject(QString uuid, QString table)
{
    QString q = "UPDATE %1 SET removed = 1 WHERE uuid = '%2';";
    threadedQuery( q.arg(table, uuid) );
}

void LocalDataInterface::restoreObject(QString uuid, QString table)
{
    QString q = "UPDATE %1 SET removed = 0 WHERE uuid = '%2';";
    threadedQuery( q.arg(table, uuid) );
}

bool LocalDataInterface::isRemoved(QString uuid, QString table)
{
    QString q = "SELECT removed FROM %1 WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    if (qry.first())
    {
        return qry.value(0).toBool();
    }

    return true;
}

void LocalDataInterface::setUsername(QString uuid, QString username)
{
    username.replace("'", "''");

    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "INSERT INTO RamUser (userName, modified, uuid) "
                "VALUES ('%1', '%2', '%3') "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET userName=excluded.userName, modified=excluded.modified ;";
    threadedQuery( q.arg(username, modified.toString("yyyy-MM-dd hh:mm:ss:zzz"), uuid) );
}

ServerConfig LocalDataInterface::serverConfig()
{
    QString q = "SELECT address, useSsl, updateDelay, timeout FROM RamServer;";
    QSqlQuery qry = query( q );

    ServerConfig config;
    if (qry.first()) {
        config.address = qry.value(0).toString();
        config.useSsl = qry.value(1).toBool();
        config.updateDelay = qry.value(2).toInt();
        config.timeout = qry.value(3).toInt();
    }

    return config;
}

const QString &LocalDataInterface::dataFile() const
{
    return m_dataFile;
}

ServerConfig LocalDataInterface::setDataFile(const QString &file)
{
    /*QSqlDatabase db = QSqlDatabase::database("localdata");
    // Set the SQLite file
    db.close();
    // Open
    db.setDatabaseName(file);
    if (!db.open()) log("Can't save data to the disk.", DuQFLog::Fatal);*/

    ProcessManager *pm = ProcessManager::instance();
    pm->start();
    pm->setMaximum(10);
    pm->setTitle(tr("Loading database"));
    pm->setText(tr("Opening database..."));

    m_querier->setDataFile(file);

    pm->increment();
    pm->setText(tr("Preparing database handling thread..."));

    emit newDataFile(file);

    pm->increment();
    pm->setText(tr("Loading data..."));

    m_dataFile = file;    

    emit dataReset();

    pm->increment();
    pm->setText(tr("Ready!"));
    pm->finish();

    return serverConfig();
}

void LocalDataInterface::sync(QJsonArray tables)
{
    for (int i = 0; i < tables.count(); i++)
    {
        QJsonObject table = tables.at(i).toObject();
        QString tableName = table.value("name").toString();
        if (tableName == "") continue;
        QJsonArray incomingRows = table.value("modifiedRows").toArray();

        // Insert new
        for (int r = incomingRows.count() - 1; r >= 0; r--)
        {
            QJsonObject incomingRow = incomingRows.at(r).toObject();
            qDebug() << "Incoming Row:";
            qDebug() << incomingRow;

            // Check if row exists
            QString uuid = incomingRow.value("uuid").toString();
            if (uuid == "")
            {
                incomingRows.removeAt(r);
                continue;
            }
            if (contains(uuid, tableName)) continue;


            QString data = incomingRow.value("data").toString().replace("'", "''");
            QString modified = incomingRow.value("modified").toString();
            QString removed = incomingRow.value("removed").toString("0");

            if (tableName == "RamUser")
            {
                QString userName = incomingRow.value("userName").toString().replace("'", "''");
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                QString q = "INSERT INTO %1 (data, modified, uuid, removed, userName) "
                            "VALUES ( '%2', '%3', '%4', %5, '%6' );";

                threadedQuery( q.arg(tableName, data, modified, uuid, removed, userName) );
            }
            else
            {
                QString q = "INSERT INTO %1 (data, modified, uuid, removed) "
                            "VALUES ( '%2', '%3', '%4', %5 );";

                threadedQuery( q.arg(tableName, data, modified, uuid, removed) );
            }

            incomingRows.removeAt(r);

            // TODO
            // warn containing list
        }

        // Update existing
        for (int r = incomingRows.count() - 1; r >= 0; r--)
        {
            QJsonObject incomingRow = incomingRows.at(r).toObject();
            QString uuid = incomingRow.value("uuid").toString();
            QString data = incomingRow.value("data").toString().replace("'", "''");
            QString modified = incomingRow.value("modified").toString();
            QString removed = incomingRow.value("removed").toString("0");
            if (tableName == "RamUser")
            {
                QString userName = incomingRow.value("userName").toString().replace("'", "''");
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                QString q = "UPDATE %1 SET"
                            "data = '%2',"
                            "modified = '%3',"
                            "removed = %4,"
                            "userName = '%5'"
                            "WHERE uuid = '%6';";

                threadedQuery( q.arg(tableName, data, modified, removed, userName, uuid) );
            }
            else
            {
                QString q = "UPDATE %1 SET"
                            "data = '%2',"
                            "modified = '%3',"
                            "removed = %4,"
                            "WHERE uuid = '%5';";

                threadedQuery( q.arg(tableName, data, modified, removed, uuid) );
            }

            // TODO
            // warn existing object
        }
    }
}

QStringList LocalDataInterface::tableNames()
{
    QSqlDatabase db = QSqlDatabase::database("infodb");

    // Copy the template to a file we can read
    QString tempDB = FileUtils::copyToTemporary(":/data/template");
    db.setDatabaseName(tempDB);

    if (!db.open())
    {
        qDebug() << "Can't open template DB";
        return QStringList();
    }

    // Get info
    QSqlQuery qry = QSqlQuery(db);

    if (!qry.exec("SELECT name FROM sqlite_schema WHERE type ='table' AND name NOT LIKE 'sqlite_%';"))
    {
        qDebug() << "Can't query template DB";
        return QStringList();
    }

    QStringList tables;
    while (qry.next())
    {
        QString name = qry.value(0).toString();
        if (name.startsWith("Ram")) tables << name;
    }

    db.close();

    // Remove the temp file
    QFile::remove(tempDB);

    return tables;
}

void LocalDataInterface::logError(QString err)
{
    log(err, DuQFLog::Critical);
}

void LocalDataInterface::finishQuery(QString q)
{
    m_activeQueries.removeOne(q);
    if (m_activeQueries.isEmpty()) {
        emit ready();
    }
}

LocalDataInterface::LocalDataInterface() :
    DuQFLoggerObject("Local Data Interface")
{
    m_querier = new Querier("getter");
    m_tQuerier = new Querier("setter");
    m_tQuerier->moveToThread(&m_queryThread);

    connect(m_querier, &Querier::error, this, &LocalDataInterface::logError);
    connect(m_tQuerier, &Querier::error, this, &LocalDataInterface::logError);
    connect(this, &LocalDataInterface::newQuery, m_tQuerier, &Querier::query);
    connect(this, &LocalDataInterface::newDataFile, m_tQuerier, &Querier::setDataFile);
    connect(m_tQuerier, &Querier::queryFinished, this, &LocalDataInterface::finishQuery);

    QSqlDatabase editdb = QSqlDatabase::addDatabase("QSQLITE","editdb");
    editdb.setHostName("localhost");
    QSqlDatabase infodb = QSqlDatabase::addDatabase("QSQLITE","infodb");
    infodb.setHostName("localhost");

    m_queryThread.start();
}

QSqlQuery LocalDataInterface::query(QString q)
{
    return m_querier->query(q);
}

void LocalDataInterface::threadedQuery(QString q)
{
    emit newQuery(q);
    m_activeQueries << q;
}

