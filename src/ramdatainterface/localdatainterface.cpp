#include "localdatainterface.h"

#include "datacrypto.h"
#include "duqf-app/app-version.h"
#include "duqf-utils/utils.h"
#include "progressmanager.h"
#include "ramuser.h"
#include "ramses.h"

// INTERFACE

QSet<QString> LocalDataInterface::projectTableNames = {
        "RamAsset",
        "RamAssetGroup",
        "RamPipe",
        "RamPipeFile",
        "RamScheduleComment",
        "RamScheduleEntry",
        "RamSequence",
        "RamShot",
        "RamStatus",
        "RamStep"
    };

QSet<QString> LocalDataInterface::generalTableNames = {
        "RamApplication",
        "RamFileType",
        "RamProject",
        "RamState",
        "RamTemplateAssetGroup",
        "RamTemplateStep",
        "RamUser"
    };

LocalDataInterface *LocalDataInterface::_instance = nullptr;

LocalDataInterface *LocalDataInterface::instance()
{
    if (!_instance) _instance = new LocalDataInterface();
    return _instance;
}

void LocalDataInterface::setServerSettings(QString dbFile, ServerConfig c)
{
    // Make sure the interface is ready
    LocalDataInterface::instance();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile)) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

    // Remove previous settings
    QSqlQuery qry = QSqlQuery(db);
    if (!qry.exec("DELETE FROM _Server;"))
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
    QString q = "INSERT INTO _Server (address, useSsl, updateDelay, timeout, port) "
            "VALUES ('%1', %2, %3, %4, %5)";

    QString useSsl = "1";
    if (!c.useSsl) useSsl = "0";
    if (!qry.exec(q.arg(c.address, useSsl, QString::number(c.updateDelay), QString::number(c.timeout), QString::number(c.port))))
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
    if (!openDB(db, dbFile)) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

    // Get settings
    QSqlQuery qry = QSqlQuery(db);

    if (!qry.exec("SELECT address, useSsl, updateDelay, timeout, port FROM _Server;"))
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
        s.port = qry.value(4).toInt();
    }

    db.close();

    return s;
}

void LocalDataInterface::setRamsesPath(QString dbFile, QString p)
{
    // Make sure the interface is ready
    LocalDataInterface::instance();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile)) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

    QSqlQuery qry = QSqlQuery(db);

    // Add new settings
    QString q = "INSERT INTO _Paths (path, name) "
                "VALUES ('%1', 'Ramses') "
                "ON CONFLICT(name) DO UPDATE "
                "SET path=excluded.path ;";

    if (!qry.exec(q.arg(p)))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
    }

    db.close();
}

QString LocalDataInterface::getRamsesPath(QString dbFile)
{
    // Make sure the interface is ready
    LocalDataInterface::instance();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    // Set the SQLite file
    if (!openDB(db, dbFile)) LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);

        QSqlQuery qry = QSqlQuery( db );
    if (!qry.exec("SELECT path FROM _Paths WHERE name = 'Ramses';"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);

        db.close();
        return "auto";
    }

    if (qry.first())
    {
        return qry.value(0).toString();
    }

    db.close();

    return "auto";
}

QSet<QString> LocalDataInterface::tableUuids(QString table, bool includeRemoved, QString projectUuid)
{
    // If we've got the info in the cache, use it.
    if (CACHE_LOCAL_DATA && m_uuids.contains(table) ) return m_uuids.value(table);

    QString q = "SELECT uuid FROM '%1'";
    if (!includeRemoved) q += " WHERE removed = 0";
    if (projectUuid != "") q += " AND `project` = '%2'";
    q += " ;";

    if (projectUuid != "") q = q.arg(table, projectUuid);
    else q = q.arg(table);

    QSqlQuery qry = query( q );

    QSet<QString> data;

    while (qry.next()) data << qry.value(0).toString();

    // Cache
    m_uuids.insert(table, data);

    return data;
}

QVector<QStringList> LocalDataInterface::tableData(QString table, bool includeRemoved, QString projectUuid)
{
    QString q = "SELECT `uuid`, `data`, `project` FROM '%1'";
    if (!includeRemoved) q += " WHERE `removed` = 0";
    if (projectUuid != "") q += " AND `project` = '%2'";
    q += " ;";

    if (projectUuid != "") q = q.arg(table, projectUuid);
    else q = q.arg(table);

    QSqlQuery qry = query( q );

    QVector<QStringList> data;

    while (qry.next())
    {
        QStringList entry;
        entry << qry.value(0).toString();
        entry << qry.value(1).toString();
        entry << qry.value(2).toString();
        data << entry;
    }

    return data;
}

bool LocalDataInterface::contains(QString uuid, QString table)
{
    // Get all UUIDS
    QSet<QString> uuids = tableUuids(table, true);
    return uuids.contains(uuid);


    /*QString q = "SELECT uuid FROM '%1' WHERE uuid = '%2';";
    q = q.arg(table, uuid);
    QSqlQuery qry = query( q );

    if (qry.first() && qry.value(0) != "") return true;
    return false;*/
}

QMap<QString, QString> LocalDataInterface::modificationDates(QString table, QString projectUuid)
{
    QString q = "SELECT uuid, modified FROM '%1'";
    if (projectUuid != "") q += " WHERE `project` = '%2'";
    q += " ;";

    if (projectUuid != "") q = q.arg(table, projectUuid);
    else q = q.arg(table);

    QSqlQuery qry = query( q );
    QMap<QString, QString> dates;
    while(qry.next())
    {
        dates[ qry.value(0).toString() ] = qry.value(1).toString();
    }
    return dates;
}

void LocalDataInterface::createObject(QString uuid, QString table, QString data, QString projectUuid, bool removed)
{
    // Remove table cache
    m_uuids.remove(table);

    data.replace("'", "''");

    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "INSERT INTO '%1' (uuid, data, modified, removed, project) "
                "VALUES ('%2', '%3', '%4', %5, '%6') "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified, project=excluded.project, removed=excluded.removed ;";

    QString rem = "0";
    if (removed) rem = "1";

    query( q.arg(
                  table,
                  uuid,
                  data,
                  modified.toString("yyyy-MM-dd hh:mm:ss"),
                  rem,
                  projectUuid
                  )
            );

    emit inserted(uuid, table);
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

    query( q.arg(table, data, modified.toString("yyyy-MM-dd hh:mm:ss"), uuid) );
    emit dataChanged(uuid);
}

QString LocalDataInterface::project(QString uuid, QString table)
{
    QString q = "SELECT `project`, `data` FROM `%1` WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    QString project = "";
    if (qry.first())
    {
        project = qry.value(0).toString();

        if (project != "") return project;

        // If it's a project table, try with the data
        if (projectTableNames.contains(table))
        {
            QString data = qry.value(1).toString();
            QSqlDatabase db = QSqlDatabase::database("localdata");
            project = findProjectUuid(data, table, db);
            if (project != "") setProject(uuid, table, project);
            return project;
        }
    }

    return "";
}

void LocalDataInterface::setProject(QString uuid, QString table, QString projectUuid)
{
    QString q = "UPDATE `%1` SET `project` = '%2' WHERE `uuid` = '%3';";
    query( q.arg(table, projectUuid, uuid) );
}

void LocalDataInterface::removeObject(QString uuid, QString table)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "UPDATE %1 SET "
                "removed = 1,"
                "modified = '%2' "
                "WHERE uuid = '%3';";
    query( q.arg(table, modified.toString("yyyy-MM-dd hh:mm:ss"), uuid) );
    emit removed(uuid, table);
}

void LocalDataInterface::restoreObject(QString uuid, QString table)
{
    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "UPDATE %1 SET "
                "removed = 0,"
                "modified = '%2' "
                "WHERE uuid = '%3';";
    query( q.arg(table, modified.toString("yyyy-MM-dd hh:mm:ss"), uuid) );
    emit inserted(uuid, table);
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
    query( q.arg(username, modified.toString("yyyy-MM-dd hh:mm:ss"), uuid) );
}

bool LocalDataInterface::isUserNameAavailable(const QString &userName)
{
    QString q = "SELECT `uuid` FROM `RamUser` WHERE userName = '%1' AND removed = 0;";
    q = q.arg( userName );
    QSqlQuery qry = query( q );

    if (qry.first() && qry.value(0) != "") return false;
    return true;
}

void LocalDataInterface::updateUser(QString uuid, QString username, QString data, QString modified)
{
    // Adjust modified if not provided
    if (modified == "")
    {
        QDateTime m = QDateTime::currentDateTimeUtc();
        modified = m.toString("yyyy-MM-dd hh:mm:ss");
    }

    // Encrypt data
    if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
    else data.replace("'", "''");

    // Insert/update
    QString q = "INSERT INTO RamUser (data, modified, uuid, userName, removed) "
                "VALUES ( '%1', '%2', '%3', '%4', 0) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified, userName=excluded.userName, removed=0 ;";

    query( q.arg(data, modified, uuid, username) );

    // Remove duplicates if any. This should never happen,
    // but when messing around with new databases and server install
    // Users may end up connecting to a new server while already having some users locally
    QDateTime m = QDateTime::currentDateTimeUtc();
    modified = m.toString("yyyy-MM-dd hh:mm:ss");
    q = "UPDATE RamUser SET `removed` = 1, `modified` = '%1' WHERE `userName` = '%2' AND `uuid` != '%3';";
    query( q.arg(modified, username, uuid) );
}

ServerConfig LocalDataInterface::serverConfig()
{
    QString q = "SELECT address, useSsl, updateDelay, timeout, port FROM _Server;";
    QSqlQuery qry = query( q );

    ServerConfig config;
    if (qry.first()) {
        config.address = qry.value(0).toString();
        config.useSsl = qry.value(1).toBool();
        config.updateDelay = qry.value(2).toInt();
        config.timeout = qry.value(3).toInt();
        config.port = qry.value(4).toInt();
    }

    return config;
}

void LocalDataInterface::setServerUuid(QString serverUuid)
{
    QString q = "UPDATE _Sync SET uuid = '%1';";
    query( q.arg(serverUuid) );
}

QString LocalDataInterface::serverUuid()
{
    QString q = "SELECT uuid FROM _Sync;";
    QSqlQuery qry = query( q );
    QString uuid = "";
    if (qry.first()) {
        uuid = qry.value(0).toString();
    }
    return uuid;
}

QString LocalDataInterface::ramsesPath()
{
    QString q = "SELECT path FROM _Paths WHERE name = 'Ramses';";
    QSqlQuery qry = query( q );
    if (qry.first())
    {
        return qry.value(0).toString();
    }
    return "auto";
}

void LocalDataInterface::setRamsesPath(QString path)
{
    QString q = "INSERT INTO _Paths (path, name) "
                "VALUES ('%1', 'Ramses') "
                "ON CONFLICT(name) DO UPDATE "
                "SET path=excluded.path ;";
    query( q.arg(path) );

    emit ramsesPathChanged(path);
}

const QString &LocalDataInterface::dataFile() const
{
    return m_dataFile;
}

ServerConfig LocalDataInterface::setDataFile(const QString &file)
{
    // Clear all cache
    m_uuids.clear();

    ProgressManager *pm = ProgressManager::instance();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    openDB(db, file);

    pm->increment();
    pm->setText(tr("Loading data..."));

    m_dataFile = file;

    emit dataReset();

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

    pm->increment();

    return serverConfig();
}

QJsonObject LocalDataInterface::getQuickSync()
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("Quick data sync: Getting local data..."));
    pm->increment();

    // List all tables
    QSet<QString> tNames = tableNames();

    QJsonObject result = getSync(tNames, true);

    pm->setText(tr("Successfully scanned local data."));
    pm->increment();

    return result;
    //emit readyToSync(tables, lastSync);
}

QJsonObject LocalDataInterface::getGeneralSync()
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("General data sync: Getting local data..."));
    pm->increment();

    // List general tables
    QJsonObject result = getSync(generalTableNames, false);

    pm->setText(tr("Successfully scanned local data."));
    pm->increment();

    return result;
    //emit readyToSync(tables, lastSync);
}

QJsonObject LocalDataInterface::getProjectSync(QString projectUuid)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("Project data sync: Getting local data..."));
    pm->increment();

    // List project tables
    QJsonObject result = getSync(projectTableNames, false, projectUuid);

    pm->setText(tr("Successfully scanned local data."));
    pm->increment();

    return result;
    //emit readyToSync(tables, lastSync);
}

QJsonObject LocalDataInterface::getSync(QSet<QString> tables, bool quick, QString projectUuid)
{
    ProgressManager *pm = ProgressManager::instance();

    // Get last Sync
    QString lastSync = "1818-05-05 00:00:00";
    QString q = "SELECT lastSync FROM _Sync ;";
    QSqlQuery qry = query(q);
    if (qry.first()) lastSync = qry.value(0).toString();

    QJsonArray tablesArray;

    // For each table, get modified rows

    RamUser *u = Ramses::instance()->currentUser();
    QString currentUuid = "";
    if (u) currentUuid = u->uuid();

    pm->addToMaximum(tables.count());

    foreach (QString tName, tables)
    {
        pm->setText(tr("Scanning table: %1").arg(tName));
        pm->increment();

        QJsonObject table;
        QJsonArray rows;
        table.insert("name", tName );

        if (tName == "RamUser") q = "SELECT uuid, data, project, modified, removed, userName FROM %1 ";
        else q = "SELECT uuid, data, project, modified, removed FROM %1 ";
        if (quick) q += " WHERE modified >= '%2' ";

        if (quick && projectUuid != "") q += " AND ";
        else if (projectUuid != "") q += " WHERE ";

        if (projectUuid != "") q += "`project` = '" + projectUuid + "' ";

        q += ";";

        if (quick) q = q.arg(tName, lastSync);
        else q = q.arg(tName);

        qry = query( q );

        while (qry.next())
        {
            QJsonObject obj;
            obj.insert("uuid", qry.value(0).toString() );
            obj.insert("project", qry.value(2).toString() );
            obj.insert("modified", qry.value(3).toString() );
            obj.insert("removed", qry.value(4).toInt() );
            QString data = qry.value(1).toString();
            if (tName == "RamUser")
            {
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientDecrypt( data );
                obj.insert("userName", qry.value(5).toString());
            }

            obj.insert("data", data );
            rows.append(obj);
        }

        table.insert("modifiedRows", rows);
        tablesArray.append(table);
    }

    QJsonObject result;
    result.insert("tables", tablesArray);
    result.insert("previousSyncDate", lastSync);
    result.insert("project", projectUuid);

    return result;
}

void LocalDataInterface::saveSync(QJsonArray tables)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(tables.count()*3);

    // Insertions
    for (int i = 0; i < tables.count(); i++)
    {
        pm->increment();

        QJsonObject table = tables.at(i).toObject();
        QString tableName = table.value("name").toString();
        if (tableName == "") continue;

        pm->setText(tr("Inserting new data in: %1").arg(tableName));

        // Clear cache
        m_uuids.remove(tableName);

        QJsonArray incomingRows = table.value("modifiedRows").toArray();

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // Insert new
        QSet<QStringList> insertedObjects;
        for (int r = incomingRows.count() - 1; r >= 0; r--)
        {
            QJsonObject incomingRow = incomingRows.at(r).toObject();

            // Check if row exists
            QString uuid = incomingRow.value("uuid").toString();
            if (uuid == "")
            {
                incomingRows.removeAt(r);
                continue;
            }

            if (uuidDates.contains(uuid)) continue;

            QString data = incomingRow.value("data").toString();
            QString modified = incomingRow.value("modified").toString();
            QString project = incomingRow.value("project").toString();
            int removed = incomingRow.value("removed").toInt(0);

            if (tableName == "RamUser")
            {
                QString userName = incomingRow.value("userName").toString().replace("'", "''");
                if (userName == "Ramses") continue;
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                else data.replace("'", "''");

                QString q = "INSERT INTO %1 (data, modified, uuid, removed, userName, project) "
                            "VALUES ( '%2', '%3', '%4', %5, '%6', '%7' );";

                query( q.arg(tableName, data, modified, uuid, QString::number(removed), userName, project) );
            }
            else
            {
                data.replace("'", "''");
                QString q = "INSERT INTO %1 (data, modified, uuid, removed, project) "
                            "VALUES ( '%2', '%3', '%4', %5, '%6' );";

                query( q.arg(tableName, data, modified, uuid, QString::number(removed), project) );
            }

            if (removed == 0)
            {
                QStringList ins;
                ins << uuid << tableName;
                insertedObjects << ins;
            }

            incomingRows.removeAt(r);
        }

        // Emit insertions
        foreach(QStringList io, insertedObjects ) emit inserted( io.at(0), io.at(1) );
    }

    // Updates
    for (int i = 0; i < tables.count(); i++)
    {
        pm->increment();

        QJsonObject table = tables.at(i).toObject();
        QString tableName = table.value("name").toString();
        if (tableName == "") continue;

        pm->setText(tr("Updating existing data for: %1").arg(tableName));

        // Clear cache
        m_uuids.remove(tableName);

        QJsonArray incomingRows = table.value("modifiedRows").toArray();

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // Update existing
        for (int r = incomingRows.count() - 1; r >= 0; r--)
        {
            QJsonObject incomingRow = incomingRows.at(r).toObject();

            QString uuid = incomingRow.value("uuid").toString();
            QString data = incomingRow.value("data").toString();
            QString project = incomingRow.value("project").toString();
            QString modified = incomingRow.value("modified").toString();
            int rem = incomingRow.value("removed").toInt(0);
            bool hasBeenRemoved = rem == 1;

            // Only if more recent
            QDateTime incomingDate = QDateTime::fromString(modified, "yyyy-MM-dd hh:mm:ss");
            QDateTime currentDate = QDateTime::fromString(uuidDates[uuid], "yyyy-MM-dd hh:mm:ss");
            if (incomingDate <= currentDate) continue;

            // Check if the object has been removed or restored
            bool wasRemoved = isRemoved(uuid, tableName);
            bool availChanged = wasRemoved != hasBeenRemoved;
            if (availChanged) {
                emit availabilityChanged(uuid, !hasBeenRemoved);
                if (hasBeenRemoved) emit removed(uuid, tableName);
            }

            if (tableName == "RamUser")
            {
                QString userName = incomingRow.value("userName").toString().replace("'", "''");

                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                else data.replace("'", "''");
                QString q = "UPDATE %1 SET "
                            "data = '%2', "
                            "modified = '%3', "
                            "removed = %4, "
                            "userName = '%5', "
                            "project = '%6' "
                            "WHERE uuid = '%7' ;";

                query( q.arg(tableName, data, modified, QString::number(rem), userName, project, uuid) );
            }
            else
            {
                data.replace("'", "''");
                QString q = "UPDATE %1 SET "
                            "data = '%2', "
                            "modified = '%3', "
                            "removed = %4, "
                            "project = '%5' "
                            "WHERE uuid = '%6' ;";

                query( q.arg(tableName, data, modified, QString::number(rem), project, uuid) );
            }

            emit dataChanged(uuid);
        }
    }

    emit synced();
}

void LocalDataInterface::deleteData(QJsonArray tables)
{
    ProgressManager *pm = ProgressManager::instance();

    for (int i = 0; i < tables.count(); i++)
    {
        pm->increment();

        QJsonObject table = tables.at(i).toObject();
        QString tableName = table.value("name").toString();
        QJsonArray rows = table.value("rows").toArray();

        pm->setText(tr("Removing out-of-date data from: %1").arg(tableName));

        for (int j = 0; j < rows.count(); j++)
        {
            QString uuid = rows.at(j).toString();
            QString q = "DELETE FROM %1 WHERE `uuid` = '%2';";
            query( q.arg( tableName, uuid ) );
        }
    }
}

QString LocalDataInterface::currentUserUuid()
{
    QString q = "SELECT uuid FROM _User WHERE current = 1;";
    QSqlQuery qry = query( q );

    if (qry.first()) return qry.value(0).toString();
    return "";
}

void LocalDataInterface::setCurrentUserUuid(QString uuid)
{
    // Set everyone to not current
    query( "UPDATE _User SET current = 0 ;" );

    QString q = "INSERT INTO _User (uuid, current) "
                "VALUES ('%1', 1 ) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET current=excluded.current ;";

    query( q.arg(uuid) );
}

void LocalDataInterface::sync(QJsonObject data, QString serverUuid)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->reInit();
    pm->setTitle(tr("Saving Ramses server data."));
    pm->setText(tr("Updating local data..."));
    pm->setMaximum(3);
    pm->start();

    saveSync(data.value("tables").toArray());

    deleteData(data.value("deletedData").toArray());

    // Save sync date

    pm->increment();
    pm->setText(tr("Cleaning..."));

    QString q = "DELETE FROM _Sync;";
    query( q );
    q = "INSERT INTO _Sync ( lastSync, uuid ) VALUES ( '%1', '%2' );";
    query( q.arg( QDateTime::currentDateTimeUtc().toString("yyyy-MM-dd hh:mm:ss"), serverUuid ) );

    pm->finish();
}

QSet<QString> LocalDataInterface::tableNames()
{
    QSet<QString> tables = projectTableNames;
    return tables.unite(generalTableNames);
    /*QSqlDatabase db = QSqlDatabase::database("infodb");
    db.close();

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

    return tables;*/
}

QVector<QStringList> LocalDataInterface::users()
{
    QString q = "SELECT uuid, userName FROM RamUSer ORDER BY userName;";
    QSqlQuery qry = query( q );
    QVector<QStringList> us;
    while(qry.next())
    {
        QStringList u;
        u << qry.value(0).toString() << qry.value(1).toString();
        us << u;
    }
    return us;
}

QString LocalDataInterface::cleanDataBase(int deleteDataOlderThan)
{
    QString report = "";

    // Backup the DB File
    QFileInfo dbFileInfo(m_dataFile);
    QString backupFile = dbFileInfo.path() + "/" + dbFileInfo.baseName() + "_bak." + dbFileInfo.completeSuffix();
    if (QFileInfo::exists(backupFile)) FileUtils::remove(backupFile);
    FileUtils::copy(m_dataFile, backupFile);

    // Clear cache
    m_uuids.clear();

    // Get needed data

    // Users (not removed)
    QSet<QString> userUuids = tableUuids("RamUser", false);

    // 1- Clean Statuses
    report += "# Cleaning report\n\n";
    report += ".\n\n## Status\n\n";
    int numStatusChanged = 0;
    QVector<QStringList> statusData = tableData("RamStatus", true);
    for (int i = 0; i < statusData.count(); i++)
    {
        QJsonDocument d = QJsonDocument::fromJson( statusData[i][1].toUtf8() );
        QJsonObject o = d.object();
        bool changed = false;

        // Check user UUID
        QString userUuid = o.value("user").toString( "" );
        // If nothing, or if this user does not exists or has been removed,
        // it's "none" (the Ramses user)
        if (userUuid == "" || ( !userUuids.contains(userUuid) && userUuid != "none") ) {
            //report += "- **" + o.value("shortName").toString() + "**: Removed invalid user.\n";
            o.insert("user", "none");
            changed = true;
        }

        // If the status has been changed, update it's data
        if (changed) {
            numStatusChanged++;
            d.setObject(o);
            QString uuid = statusData[i][0];
            qDebug() << ">>> Updating data for status " + uuid;
            setObjectData( uuid, "RamStatus", d.toJson(QJsonDocument::Compact) );
        }
    }
    if (numStatusChanged == 0) report += "*Everything is fine.*\n\n";
    else report += "**" + QString::number(numStatusChanged) + " status** were updated.\n\n";

    // 2- Get removed rows older than limit
    if (deleteDataOlderThan >= 0)
    {
        report += ".\n\n## Deleted Data\n\n";
        qDebug() << ">>> Collecting data older than " << deleteDataOlderThan << " days.";

        QString limitDate = QDateTime::currentDateTimeUtc().addDays(-deleteDataOlderThan).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "    Date: " << limitDate;

        int dataRemoved = false;
        m_uuidsToRemove.clear();

        // For each table, get UUIDs to clean
        QSet<QString> tables = tableNames();
        foreach(QString table, tables)
        {
            qDebug() << "    From: " << table;

            // List uuids to remove
            QString q = "SELECT uuid FROM %1 WHERE `removed` = 1 AND `modified` <= '%2'";
            QSqlQuery qry = query( q.arg(table, limitDate) );

            // Collect and remove uuids
            QSet<QString> uuids;
            while (qry.next()) {
                QString uuid = qry.value(0).toString();
                q = "DELETE FROM %1 WHERE `uuid` = '%2' AND `removed` = 1;";
                query( q.arg(table, uuid) );
                uuids.insert( uuid );
            }

            int count = uuids.count();
            if (count > 0) {
                qDebug() << "    count: " << count;
                dataRemoved = true;
                m_uuidsToRemove.insert(table, uuids);
                report += "Deleted " + QString::number(count) + " objects from " + table + ".\n\n";
            }
        }
        if (!dataRemoved) report += "*Nothing was found to delete.*\n\n";
    }

    // Vacuum
    vacuum();
    report += ".\n\n## Maintenance\n\n";
    report += "Deleted and orphan data has been removed from disk,  \nthe database size has been shrinked to its minimum size.\n\n";

    qDebug() << "Finished clean.";

    return report;
}

bool LocalDataInterface::undoClean()
{
    // Restore the backuped file
    QFileInfo dbFileInfo(m_dataFile);
    QString backupFile = dbFileInfo.path() + "/" + dbFileInfo.baseName() + "_bak." + dbFileInfo.completeSuffix();
    if (!QFileInfo::exists(backupFile)) return false;

    // Unset the database file
    QSqlDatabase db = QSqlDatabase::database("localdata");
    db.close();

    FileUtils::moveToTrash(m_dataFile);
    FileUtils::copy(backupFile, m_dataFile);

    return true;
}

void LocalDataInterface::logError(QString err)
{
    log(err, DuQFLog::Critical);
}

void LocalDataInterface::quit()
{
    qDebug() << "LocalDataInterface: Vacuuming...";
    vacuum();
    //waitForReady();
    qDebug() << "LocalDataInterface: Everything's clean.";
}

LocalDataInterface::LocalDataInterface():
    DuQFLoggerObject("Local Data Interface")
{
    //Load local database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","localdata");
    db.setHostName("localhost");

    QSqlDatabase editdb = QSqlDatabase::addDatabase("QSQLITE","editdb");
    editdb.setHostName("localhost");
    QSqlDatabase infodb = QSqlDatabase::addDatabase("QSQLITE","infodb");
    infodb.setHostName("localhost");

    //m_queryThread.start();

    connect(qApp, &QApplication::aboutToQuit, this, &LocalDataInterface::quit);
}

bool LocalDataInterface::openDB(QSqlDatabase db, const QString &dbFile)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setTitle(tr("Loading database"));
    pm->setText(tr("Opening database..."));

    // Make sure the DB is closed
    db.close();
    // Open
    db.setDatabaseName(dbFile);
    if (!db.open()) {
        LocalDataInterface::instance()->log("Can't save data to the disk.", DuQFLog::Fatal);
        return false;
    }

    // Check the version, and update the db scheme
    pm->setText(tr("Checking database version"));

    QSqlQuery qry = QSqlQuery(db);

    // Check DB Version
    qry.exec("CREATE TABLE IF NOT EXISTS _Ramses ("
                  "\"id\"	INTEGER NOT NULL UNIQUE,"
                  "\"version\"	TEXT NOT NULL DEFAULT '0.0.0',"
                  "PRIMARY KEY(\"id\" AUTOINCREMENT)"
                  ");");

    QVersionNumber currentVersion(0,0,0);
    QVersionNumber newVersion = QVersionNumber::fromString(STR_VERSION);

    qry.exec("SELECT version FROM _Ramses;");
    if (qry.first())
    {
        currentVersion = QVersionNumber::fromString( qry.value(0).toString() );
    }

    if (currentVersion < newVersion)
    {
        pm->freeze(true);
        pm->setText(tr("Upgrading database scheme"));
        QFileInfo dbFileInfo(dbFile);
        LocalDataInterface::instance()->log(tr("This database was created by an older version of Ramses (%1).\n"
               "I'm upgrading it to the current version (%2).\n"
               "The original file will be renamed to \"%3_%1.ramses\".").arg(currentVersion.toString(), STR_VERSION, dbFileInfo.baseName()));

        FileUtils::copy(dbFile, QString("%1/%2_%3.ramses").arg(
                            dbFileInfo.path(),
                            dbFileInfo.baseName(),
                            currentVersion.toString()));

        bool ok = false;
        if (currentVersion < QVersionNumber(0, 5, 1))
        {
            // Add the port entry to the _server table
            ok = qry.exec("ALTER TABLE _Server ADD COLUMN \"port\" INTEGER NOT NULL DEFAULT 443;");
            if (!ok)
            {
                QString errorMessage = "Something went wrong when updating the database scheme to the new version.\nHere's some information:";
                errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
            }
            // Set the port to the default (80/443)
            qry.exec("UPDATE _Server SET port = 443 WHERE useSsl = 1;");
            qry.exec("UPDATE _Server SET port = 80 WHERE useSsl = 0;");
        }

        if (currentVersion < QVersionNumber(0, 6, 0))
        {
            // Add the uuid entry to the _sync table
            ok = qry.exec("ALTER TABLE _Sync ADD COLUMN \"uuid\" TEXT;");
            if (!ok)
            {
                QString errorMessage = "Something went wrong when updating the database scheme to the new version.\nHere's some information:";
                errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
            }
        }

        if (currentVersion < QVersionNumber(0, 7, 0))
        {
            // Add "project" column to all tables
            QSet<QString> tables = tableNames();

            pm->addToMaximum(tables.count());

            foreach( QString table, tables )
            {
                QString tn = table;
                pm->setText(tr("Upgrading %1 table (this may take a while...)").arg(tn.replace("Ram", "")));
                pm->increment();

                QString q = "ALTER TABLE `%1` ADD COLUMN \"project\" TEXT;";
                ok = qry.exec( q.arg( table ) );
                if (!ok)
                {
                    QString errorMessage = "Something went wrong when updating the database scheme to the new version.\nHere's some information:";
                    errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                    errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                    errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                    LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
                    break;
                }

                // Not a project table, that's all
                if (!projectTableNames.contains(table)) continue;

                // Find the project, and copy info to the column
                q = "SELECT uuid, data FROM `%1`";
                ok = qry.exec( q.arg( table ) );
                if (!ok)
                {
                    QString errorMessage = "Something went wrong when updating the database scheme to the new version.\nHere's some information:";
                    errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                    errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                    errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                    LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
                    break;
                }

                // Check the number of rows
                // (With SQLite, size() does not work)
                qry.last();
                int numObjs = qry.at();
                pm->addToMaximum(numObjs + 1);
                qry.seek(-1);

                while (qry.next())
                {
                    pm->increment();

                    QString uuid = qry.value(0).toString();
                    QString data = qry.value(1).toString();

                    QString projUuid = findProjectUuid(data, table, db);

                    // Set it
                    q = "UPDATE `%1` SET project = '%2' WHERE uuid = '%3';";
                    QSqlQuery qry2 = QSqlQuery(db);
                    qry2.exec( q.arg( table, projUuid, uuid ) );
                }
            }
        }

        if (ok)
        {
            // Remove previous version and update with ours
            qry.exec("DELETE FROM _Ramses;");
            QString q = "INSERT INTO _Ramses (version) VALUES ('%1');";
            if (!qry.exec(q.arg(STR_VERSION)))
            {
                QString errorMessage = "Something went wrong when setting the new version of the database.\nHere's some information:";
                errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                LocalDataInterface::instance()->log(errorMessage, DuQFLog::Warning);
            }

            // Vacuum
            qry.exec("VACUUM;");

        }

        pm->setText(tr("Database upraded to version %1").arg(newVersion.toString()));
        pm->freeze(false);
    }
    else if (currentVersion > newVersion)
    {
        LocalDataInterface::instance()->log(tr("This database was created by a more recent version of Ramses (%1).\n"
               "You should update this Ramses application before using this database!\n"
               "Be careful if you continue, this could lead to data loss or corrupted databases.").arg(currentVersion.toString()),
            DuQFLog::Critical);
    }

    return true;
}

QString LocalDataInterface::findProjectUuid(QString data, QString table, QSqlDatabase db)
{
    // Find project uuid
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj = doc.object();
    QString projUuid = obj.value("project").toString();

    if (projUuid != "") return projUuid;

    // For some tables, find the project info in another table
    QString otherTable = "";
    QString objKey = "";
    if (table == "RamScheduleEntry" || table == "RamStatus")
    {
        otherTable = "RamStep";
        objKey = "step";
    }
    else if (table == "RamPipe")
    {
        otherTable = "RamStep";
        objKey = "inputStep";
    }

    QString otherUuid = obj.value(objKey).toString();
    if (otherUuid != "")
    {
        QSqlQuery qry2 = QSqlQuery(db);

        QString q = "SELECT data FROM `%1` WHERE uuid = '%2';";
        if (qry2.exec(q.arg(otherTable, otherUuid)))
        {
            if (qry2.next())
            {
                QString otherData = qry2.value(0).toString();
                doc = QJsonDocument::fromJson(otherData.toUtf8());
                obj = doc.object();
                return obj.value("project").toString();
            }
        }
    }
    return "";
}

QSqlQuery LocalDataInterface::query(QString q) const
{
    if (m_dataFile =="") return QSqlQuery();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery qry = QSqlQuery(db);

    //log(tr("Querying:") + "\n" + q, DuQFLog::Data);
    //qDebug() << q;

    if (!qry.exec(q))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        log(errorMessage, DuQFLog::Critical);
    }

    return qry;
    //return m_querier->query(q);
}

void LocalDataInterface::vacuum()
{
    QString q = "VACUUM;";
    query( q );
}

bool LocalDataInterface::hasTable(QString tableName, QSqlDatabase db)
{
    QString q = "SELECT name FROM sqlite_master WHERE type='table' AND name='%1';";

    QSqlQuery qry = QSqlQuery(db);
    if (!qry.exec( q.arg( tableName ) )) return false;
    return qry.next();
}

const QHash<QString, QSet<QString> > &LocalDataInterface::deletedUuids() const
{
    return m_uuidsToRemove;
}

