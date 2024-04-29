#include "localdatainterface.h"

#include "datacrypto.h"
#include "datastruct.h"
#include "duqf-app/app-version.h"
#include "duqf-utils/utils.h"
#include "progressmanager.h"
#include "statemanager.h"
#include "ramuser.h"
#include "ramses.h"

// INTERFACE

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

QSet<QString> LocalDataInterface::tableUuids(QString table, bool includeRemoved)
{
    // If we've got the info in the cache, use it.
    if (includeRemoved && CACHE_LOCAL_DATA && m_uuids.contains(table) ) return m_uuids.value(table);
    if (!includeRemoved && CACHE_LOCAL_DATA && m_uuidsWithoutRemoved.contains(table) ) return m_uuidsWithoutRemoved.value(table);

    // Make sure the table exists
    createTable(table);

    QString q = "SELECT uuid FROM '%1'";
    if (!includeRemoved) q += " WHERE removed = 0";
    q += " ;";
    QSqlQuery qry = query( q.arg(table) );

    QSet<QString> data;

    while (qry.next()) data << qry.value(0).toString();

    // Cache
    if (includeRemoved) m_uuids.insert(table, data);
    else m_uuidsWithoutRemoved.insert(table, data);

    return data;
}

QVector<QStringList> LocalDataInterface::tableData(QString table, QHash<QString, QStringList> filters, bool includeRemoved)
{
    // Make sure the table exists
    createTable(table);

    QString q = "SELECT `uuid`, `data`, `modified` FROM '%1'";
    if (!includeRemoved) q += " WHERE removed = 0";

    // Add filters
    if (!filters.isEmpty())
    {
        // Iterate through the filters;
        // An STL const iterator is the fastest
        QHash<QString, QStringList>::const_iterator i = filters.constBegin();
        while (i != filters.constEnd())
        {
            QString key = i.key();
            QStringList values = i.value();

            // The data must satisfy ALL the filters
            if (key != "" && !values.isEmpty())
            {
                bool first = true;
                foreach(QString value, values)
                {
                    if (first) {
                        if (!includeRemoved) q += " AND ";
                        q += " ( ";
                    }
                    else q += " OR ";
                    first = false;
                    q += "REPLACE(`data`, ' ', '') LIKE '%\"" + key + "\":\"" + value + "\"%'";
                }
                q += " )";
            }

            i++;
        }
    }

    q += " ;";

    QSqlQuery qry = query( q.arg(table) );

    QVector<QStringList> tData;

    while (qry.next())
    {
        QString uuid = qry.value(0).toString();
        QString data = qry.value(1).toString();
        QString modified = qry.value(2).toString();

        // Decrypt data if user table
        if (table == "RamUser" && ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientDecrypt( data );

        QStringList entry;
        entry << uuid;
        entry << data;
        entry << modified;
        tData << entry;
    }

    return tData;
}

bool LocalDataInterface::contains(QString uuid, QString table, bool includeRemoved)
{
    // Get all UUIDS
    QSet<QString> uuids = tableUuids(table, includeRemoved);
    return uuids.contains(uuid);


    /*QString q = "SELECT uuid FROM '%1' WHERE uuid = '%2';";
    q = q.arg(table, uuid);
    QSqlQuery qry = query( q );

    if (qry.first() && qry.value(0) != "") return true;
    return false;*/
}

QMap<QString, QString> LocalDataInterface::modificationDates(QString table)
{
    // Make sure the table exists
    createTable(table);

    QString q = "SELECT uuid, modified FROM '%1';";
    q = q.arg(table);
    QSqlQuery qry = query( q );
    QMap<QString, QString> dates;
    while(qry.next())
    {
        dates[ qry.value(0).toString() ] = qry.value(1).toString();
    }
    return dates;
}

void LocalDataInterface::createObject(QString uuid, QString table, QString data)
{
    // Make sure the table exists
    createTable(table);

    // Remove table cache
    m_uuids.remove(table);
    m_uuidsWithoutRemoved.remove(table);

    QString newData = data;

    if (ENCRYPT_USER_DATA && table == "RamUser") newData = DataCrypto::instance()->clientEncrypt(data);
    else newData.replace("'", "''");

    QDateTime modified = QDateTime::currentDateTimeUtc();

    QString q = "INSERT INTO '%1' (uuid, data, modified, removed) "
                "VALUES ('%2', '%3', '%4', 0) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified ;";

    query( q.arg(
                  table,
                  uuid,
                  newData,
                  modified.toString("yyyy-MM-dd hh:mm:ss")
                  )
            );

    emit inserted(uuid, data, modified.toString("yyyy-MM-dd hh:mm:ss"), table);
}

QString LocalDataInterface::objectData(QString uuid, QString table)
{
    // Make sure the table exists
    createTable(table);

    QString q = "SELECT data FROM %1 WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    if (qry.first())
    {
        QString data = qry.value(0).toString();
        if (ENCRYPT_USER_DATA && table == "RamUser") data = DataCrypto::instance()->clientDecrypt(data);
        return data;
    }
    return "";
}

void LocalDataInterface::setObjectData(QString uuid, QString table, QString data)
{
    // Make sure the table exists
    createTable(table);

    QString newData = DBInterface::instance()->validateObjectData(data, uuid, table);

    QDateTime modified = QDateTime::currentDateTimeUtc();

    if (ENCRYPT_USER_DATA && table == "RamUser") newData = DataCrypto::instance()->clientEncrypt(data);
    else newData.replace("'", "''");

    QString q = "INSERT INTO %1 (data, modified, uuid) "
                "VALUES ( '%2', '%3', '%4') "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified ;";

    QString modifiedStr = modified.toString("yyyy-MM-dd hh:mm:ss");
    query( q.arg(table, newData, modifiedStr, uuid) );

    emit dataChanged(uuid, data, modifiedStr, table);
}

void LocalDataInterface::removeObject(QString uuid, QString table)
{
    // Make sure the table exists
    createTable(table);

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
    // Make sure the table exists
    createTable(table);

    QDateTime modified = QDateTime::currentDateTimeUtc();

    // Restore query
    QString q = "UPDATE %1 SET "
                "removed = 0,"
                "modified = '%2' "
                "WHERE uuid = '%3';";
    QString modifiedStr = modified.toString("yyyy-MM-dd hh:mm:ss");
    query( q.arg(table, modifiedStr, uuid) );

    // Get current data
    QString data = objectData(uuid, table);

    // Emit inserted
    emit inserted(uuid, data, modifiedStr, table);
}

bool LocalDataInterface::isRemoved(QString uuid, QString table)
{
    // Make sure the table exists
    createTable(table);

    QString q = "SELECT removed FROM %1 WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    if (qry.first())
    {
        return qry.value(0).toBool();
    }

    return true;
}

QString LocalDataInterface::modificationDate(QString uuid, QString table)
{
    // Make sure the table exists
    createTable(table);

    QString q = "SELECT modified FROM %1 WHERE uuid = '%2';";
    QSqlQuery qry = query( q.arg(table, uuid) );

    if (qry.first())
    {
        return qry.value(0).toString();
    }

    return "1818-05-05 00:00:00";
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
    QString newData = data;
    if (ENCRYPT_USER_DATA) newData = DataCrypto::instance()->clientEncrypt( newData );
    else newData.replace("'", "''");

    // Insert/update
    QString q = "INSERT INTO RamUser (data, modified, uuid, userName, removed) "
                "VALUES ( '%1', '%2', '%3', '%4', 0) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified, userName=excluded.userName, removed=0 ;";

    query( q.arg(newData, modified, uuid, username) );

    // Remove duplicates if any. This should never happen,
    // but when messing around with new databases and server install
    // Users may end up connecting to a new server while already having some users locally
    QDateTime m = QDateTime::currentDateTimeUtc();
    modified = m.toString("yyyy-MM-dd hh:mm:ss");
    q = "UPDATE RamUser SET `removed` = 1, `modified` = '%1' WHERE `userName` = '%2' AND `uuid` != '%3';";
    query( q.arg(modified, username, uuid) );

    emit dataChanged(uuid, data, modified, "RamUser");
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
    QElapsedTimer timer;
    timer.start();

    qDebug() << ">> Opening local file...";

    // Clear all cache
    m_uuids.clear();
    m_uuidsWithoutRemoved.clear();

    qDebug() << ">> Cleared cache: " << timer.elapsed()/1000 << " seconds.";

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(2);

    QSqlDatabase db = QSqlDatabase::database("localdata");
    openDB(db, file);

    qDebug() << ">> Opened file: " << timer.elapsed()/1000 << " seconds.";

    pm->increment();
    pm->setText(tr("Loading data..."));

    m_dataFile = file;

    emit dataResetCommon();
    emit dataResetProject();

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

    qDebug() << ">> Local data ready! " << timer.elapsed()/1000 << " seconds.";

    return serverConfig();
}

SyncData LocalDataInterface::getSync(bool fullSync)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->increment();

    // List all tables
    QStringList tNames = tableNames();

    // Ignore History for now to improve performance (as it is not used in Ramses yet)
    tNames.removeAll("RamStatusHistory");

    // Get last Sync
    QString lastSync = "1818-05-05 00:00:00";
    if (!fullSync)
    {
        QString q = "SELECT lastSync FROM _Sync ;";
        QSqlQuery qry = query(q);
        if (qry.first()) lastSync = qry.value(0).toString();
    }

    // For each table, get modified rows

    RamUser *u = Ramses::instance()->currentUser();
    QString currentUuid = "";
    if (u) currentUuid = u->uuid();

    pm->addToMaximum(tNames.count() + 2);

    QHash<QString, QSet<TableRow>> tables;

    for (int i = 0; i < tNames.count(); i++)
    {      
        QString tName = tNames.at(i);

        pm->setText(tr("Scanning table: %1").arg(tName));
        pm->increment();

        QSet<TableRow> rows;

        // Make sure the table exists
        createTable(tName);

        QString q;
        if (tName == "RamUser") q = "SELECT uuid, data, modified, removed, userName FROM %1 ";
        else q = "SELECT uuid, data, modified, removed FROM %1 ";
        if (!fullSync) q += " WHERE modified >= '%2' ;";

        if (!fullSync) q = q.arg(tName, lastSync);
        else q = q.arg(tName);

        QSqlQuery qry = query( q );

        while (qry.next())
        {
            TableRow row;
            row.uuid = qry.value(0).toString();
            row.modified = qry.value(2).toString();
            row.removed = qry.value(3).toInt();

            QString data = qry.value(1).toString();
            if (tName == "RamUser")
            {
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientDecrypt( data );
                row.userName = qry.value(4).toString();
            }

            row.data = data;
            rows.insert(row);
        }

        tables.insert(tName, rows);
    }

    SyncData syncData;
    syncData.syncDate = lastSync;
    syncData.tables = tables;

    pm->setText(tr("Successfully scanned local data."));
    pm->increment();

    return syncData;
}

void LocalDataInterface::saveSync(SyncData syncData)
{
    StateManager::State previousState = StateManager::i()->state();
    StateManager::i()->setState(StateManager::WritingDataBase);

    QHash<QString, QSet<TableRow>> tables = syncData.tables;

    ProgressManager *pm = ProgressManager::instance();
    pm->addToMaximum(tables.count()*2);

    // Insertions
    QHashIterator<QString, QSet<TableRow>> i(tables);

    while (i.hasNext()) {

        pm->increment();

        i.next();
        QString tableName = i.key();
        if (tableName == "") continue;

        // Make sure the table exists
        createTable(tableName);

        QSet<QStringList> insertedObjects;

        pm->setText(tr("Inserting new data in: %1").arg(tableName));

        // Clear cache
        m_uuids.remove(tableName);
        m_uuidsWithoutRemoved.remove(tableName);

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // In a single query
        QString q;
        if (tableName == "RamUser") q = "INSERT INTO %1 (data, modified, uuid, removed, userName) VALUES ";
        else q = "INSERT INTO %1 (data, modified, uuid, removed) VALUES ";
        q = q.arg(tableName);

        QStringList values;

        QSet<TableRow> incomingRows = i.value();
        foreach(TableRow incomingRow, incomingRows)
        {
            // Check if row exists
            QString uuid = incomingRow.uuid;

            if (uuid == "") continue;

            if (uuidDates.contains(uuid)) continue;

            QString data = DBInterface::instance()->validateObjectData(incomingRow.data, uuid, tableName);

            QString modified = incomingRow.modified;
            int removed = incomingRow.removed;

            if (tableName == "RamUser")
            {
                QString userName = incomingRow.userName.replace("'", "''");
                if (userName == "Ramses") continue;
                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                else data.replace("'", "''");

                QString v = "( '%1', '%2', '%3', %4, '%5' )";
                values << v.arg( data, modified, uuid, QString::number(removed), userName );
            }
            else
            {
                data.replace("'", "''");

                QString v = "( '%1', '%2', '%3', %4 )";
                values << v.arg( data, modified, uuid, QString::number(removed) );
            }

            if (removed == 0)
            {
                QStringList ins;
                ins << uuid << incomingRow.data << modified << tableName;
                insertedObjects << ins;
            }
        }

        // Nothing, next table
        if (values.count() == 0) continue;

        q += values.join(", ");

        query( q );

        // Emit insertions
        StateManager::i()->setState(StateManager::LoadingDataBase);
        foreach(QStringList io, insertedObjects ) {
            emit inserted( io.at(0), io.at(1), io.at(2), io.at(3) );
        }
    }

    StateManager::i()->setState(StateManager::WritingDataBase);

    // Updates
    i.toFront();
    while (i.hasNext()) {
        pm->increment();

        i.next();
        QString tableName = i.key();
        if (tableName == "") continue;

        // Make sure the table exists
        createTable(tableName);

        pm->setText(tr("Updating existing data for: %1").arg(tableName));

        // Clear cache
        m_uuids.remove(tableName);
        m_uuidsWithoutRemoved.remove(tableName);

        QSet<TableRow> incomingRows = i.value();

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // In a single query
        QString q;
        if (tableName == "RamUser") q = "INSERT INTO %1 (data, modified, uuid, removed, userName) VALUES ";
        else q = "INSERT INTO %1 (data, modified, uuid, removed) VALUES ";
        q = q.arg(tableName);

        QStringList values;
        QSet<QStringList> changedUuids;

        foreach(TableRow incomingRow, incomingRows)
        {
            QString uuid = incomingRow.uuid;
            if (uuid == "") continue;
            QString data = DBInterface::instance()->validateObjectData(incomingRow.data, uuid, tableName);

            QString modified = incomingRow.modified;
            int rem = incomingRow.removed;
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
                QString userName = incomingRow.userName.replace("'", "''");

                if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientEncrypt( data );
                else data.replace("'", "''");

                QString v = "( '%1', '%2', '%3', %4, '%5' )";
                values << v.arg( data, modified, uuid, QString::number(rem), userName );
            }
            else
            {
                data.replace("'", "''");
                QString v = "( '%1', '%2', '%3', %4 )";
                values << v.arg( data, modified, uuid, QString::number(rem) );
            }

            QStringList cu;
            cu << uuid << data << modified;
            changedUuids << cu;
        }

        // Nothing, next table
        if (values.count() == 0) continue;

        q += values.join(", ") + " ON CONFLICT(uuid) DO UPDATE SET ";

        if (tableName == "RamUser") q +="`data` = excluded.data, `modified` = excluded.modified, `removed` = excluded.removed, `userName` = excluded.userName ;";
        else q += "`data` = excluded.data, `modified` = excluded.modified, `removed` = excluded.removed ;";

        query( q );

        // Emit
        StateManager::i()->setState(StateManager::LoadingDataBase);
        foreach(QStringList cu, changedUuids) {
            emit dataChanged(cu.at(0), cu.at(1), cu.at(2), tableName);
        }
    }

    emit syncFinished();

    StateManager::i()->setState(previousState);
}

void LocalDataInterface::deleteData(SyncData syncData)
{
    ProgressManager *pm = ProgressManager::instance();

    QHash<QString, QStringList> tables = syncData.deletedUuids;
    QHashIterator<QString, QStringList> i(tables);

    pm->addToMaximum(tables.count());

    while (i.hasNext()) {

        i.next();

        pm->increment();

        QString tableName = i.key();
        QStringList rows = i.value();

        pm->setText(tr("Removing out-of-date data from: %1").arg(tableName));

        for (int j = 0; j < rows.count(); j++)
        {
            QString uuid = rows.at(j);
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

void LocalDataInterface::sync(SyncData data, QString serverUuid)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setText(tr("Updating local data..."));
    pm->addToMaximum(1);

    saveSync(data);
    deleteData(data);

    // Save sync date

    pm->increment();
    pm->setText(tr("Cleaning..."));

    QString q = "DELETE FROM _Sync;";
    query( q );
    q = "INSERT INTO _Sync ( lastSync, uuid ) VALUES ( '%1', '%2' );";
    query( q.arg( data.syncDate, serverUuid ) );
}

QStringList LocalDataInterface::tableNames()
{
    QSqlDatabase db = QSqlDatabase::database("infodb");
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

    if (!qry.exec("SELECT name FROM sqlite_master WHERE type ='table' AND name NOT LIKE 'sqlite_%';"))
    {
        qDebug() << "Can't query template DB";
        qDebug() << qry.lastError().text();
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
    StateManager::State previousState = StateManager::i()->state();
    StateManager::i()->setState(StateManager::WritingDataBase);

    QString report = "";

    // Backup the DB File
    QFileInfo dbFileInfo(m_dataFile);
    QString backupFile = dbFileInfo.path() + "/" + dbFileInfo.baseName() + "_bak." + dbFileInfo.completeSuffix();
    if (QFileInfo::exists(backupFile)) FileUtils::remove(backupFile);
    FileUtils::copy(m_dataFile, backupFile);

    // Clear cache
    m_uuids.clear();
    m_uuidsWithoutRemoved.clear();

    // Get needed data

    // Users (not removed)
    QSet<QString> userUuids = tableUuids("RamUser", false);

    // 1- Clean Statuses
    report += "# Cleaning report\n\n";
    report += ".\n\n## Status\n\n";
    int numStatusChanged = 0;
    QVector<QStringList> statusData = tableData("RamStatus", QHash<QString,QStringList>(), true);
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
        QStringList tables = tableNames();
        for (int i = 0; i < tables.count(); i++) {

            QString table = tables[i];
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

    StateManager::i()->setState(previousState);

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
    pm->addToMaximum(2);
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
    pm->increment();

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

    bool ok = true;

    if (currentVersion < newVersion)
    {
        pm->setText(tr("Updating database scheme"));
        pm->increment();
        QFileInfo dbFileInfo(dbFile);
        LocalDataInterface::instance()->log(tr("This database was created by an older version of Ramses (%1).\n"
               "I'm updating it to the current version (%2).\n"
               "The original file will be renamed to \"%3_%1.ramses\".").arg(currentVersion.toString(), STR_VERSION, dbFileInfo.baseName()));

        FileUtils::copy(dbFile, QString("%1/%2_%3.ramses").arg(
                            dbFileInfo.path(),
                            dbFileInfo.baseName(),
                            currentVersion.toString()));

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

        if (currentVersion < QVersionNumber(0, 7, 2))
        {
            // Create the new RamStatusHistory table
            ok = qry.exec("CREATE TABLE IF NOT EXISTS 'RamStatusHistory' ( "
                          "\"id\"	INTEGER NOT NULL UNIQUE, "
                          "\"uuid\"	TEXT NOT NULL UNIQUE, "
                          "\"data\"	TEXT NOT NULL, "
                          "\"modified\"	timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, "
                          "\"removed\"	INTEGER NOT NULL DEFAULT 0, "
                          "PRIMARY KEY(\"id\" AUTOINCREMENT) );"
                      );
            if (!ok)
            {
                QString errorMessage = "Something went wrong when updating the database scheme to the new version.\nHere's some information:";
                errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
                errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
                errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
                LocalDataInterface::instance()->log(errorMessage, DuQFLog::Critical);
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
        }
    }
    else if (currentVersion > newVersion)
    {
        LocalDataInterface::instance()->log(tr("This database was created by a more recent version of Ramses (%1).\n"
               "You should update this Ramses application before using this database!\n"
               "Be careful if you continue, this could lead to data loss or corrupted databases.").arg(currentVersion.toString()),
            DuQFLog::Critical);
        ok = false;
    }

    // If not ok, finished
    if (!ok) return true;

    // Auto clean!
    autoCleanDB(db);

    return true;
}

void LocalDataInterface::autoCleanDB(QSqlDatabase db)
{
    ProgressManager *pm = ProgressManager::instance();
    pm->setText("Cleaning the database...");

    QSqlQuery qry = QSqlQuery(db);

    if ( !db.isOpen() )
    {
        if ( !db.open() ) return;
    }

    StateManager::State previousState = StateManager::i()->state();
    StateManager::i()->setState(StateManager::WritingDataBase);

    // === Delete removed statuses ===

    qry.exec("DELETE FROM RamStatus WHERE `removed` = 1 ;");

    // === Delete removed schedule entries ===

    qry.exec("DELETE FROM RamScheduleEntry WHERE `removed` = 1 ;");

    // === Clean Schedule entries ===

    // Add the project in schedule entries data
    qry.exec("SELECT `uuid`, `data` FROM 'RamScheduleEntry' ;");

    // Count the results to help the progress bar
    qry.last();
    int numRows = qry.at() + 1;
    pm->addToMaximum( numRows );
    qDebug() << "Found " << numRows << " RamScheduleEntry...";
    qry.seek(-1);

    // To speed up things, keep step/project association
    QHash<QString,QString> stepProjectUuids;
    QStringList uuidsToRemove;
    QVector<QStringList> updateData;
    while (qry.next())
    {
        QString dataStr = qry.value(1).toString();
        QString uuid = qry.value(0).toString();
        // Parse the data
        QJsonDocument doc = QJsonDocument::fromJson( dataStr.toUtf8() );
        QJsonObject obj = doc.object();
        QString projUuid = obj.value("project").toString();
        if (projUuid != "") continue;
        QString stepUuid = obj.value("step").toString();
        if (stepUuid == "" || stepUuid == "none") {
            uuidsToRemove << uuid;
            continue;
        }
        projUuid = stepProjectUuids.value(stepUuid);
        if (projUuid == "") {
            // Get the project
            QSqlQuery qryProj = QSqlQuery(db);
            QString q = "SELECT `data` FROM 'RamStep' WHERE `uuid` = '%1' ;";
            qryProj.exec( q.arg(stepUuid) );
            if (qryProj.first()) {
                QString stepDataStr = qryProj.value(0).toString();
                QJsonDocument stepDoc = QJsonDocument::fromJson( stepDataStr.toUtf8() );
                QJsonObject stepObj = stepDoc.object();
                projUuid = stepObj.value("project").toString();
                if (projUuid != "") stepProjectUuids.insert(stepUuid, projUuid);
            }
        }
        if (projUuid != "") {
            obj.insert("project", projUuid);
            doc.setObject(obj);
            QStringList d;
            d << uuid << doc.toJson(QJsonDocument::Compact);
            updateData << d;
        }
        else uuidsToRemove << uuid;
    }

    qDebug() << "Found " << uuidsToRemove.count() << " invalid or empty entries to remove.";
    qDebug() << "From " << updateData.count() << " entries to update with the project info.";

    // Remove uuids without project
    // Split in 250 rows at once
    if (!uuidsToRemove.isEmpty())
    {
        // Split by 250 rows at a time
        while (!uuidsToRemove.isEmpty())
        {
            QString condition = " `uuid` = '" + uuidsToRemove.takeLast() + "' ";
            for (int i = 0; i < 250; i++)
            {
                if (uuidsToRemove.isEmpty()) break;
                condition += " OR `uuid` = '" + uuidsToRemove.takeLast() + "' ";
            }

            qry.exec("DELETE FROM 'RamScheduleEntry' WHERE " + condition );
        }
    }

    // Set the new data
    if (!updateData.isEmpty())
    {
        // Split by 250 rows at a time
        while (!updateData.isEmpty())
        {
            QStringList data = updateData.takeLast();
            QString uuid = data.at(0);
            QString d = data.at(1);
            QString values = " ( '" + uuid + "', '" + d.replace("'", "''") + "' ) ";
            for (int i = 0; i < 250; i++)
            {
                if (updateData.isEmpty()) break;
                data = updateData.takeLast();
                QString uuid = data.at(0);
                QString d = data.at(1);
                values += ", ( '" + uuid + "', '" + d.replace("'", "''") + "' ) ";
            }

            qry.exec("INSERT INTO RamScheduleEntry (`uuid`, `data`) "
                     "VALUES " + values +
                     " ON CONFLICT(uuid) DO UPDATE SET "
                     " `data` = excluded.data ;"
                     );
        }
    }

    // === Move the oldest statuses to their statushistory table ===

    // List all statuses
    qry.exec("SELECT `uuid`, `data` FROM RamStatus ;");

    // Count the results to help the progress bar
    qry.last();
    numRows = qry.at() + 1;
    pm->addToMaximum( numRows );
    qDebug() << "Found " << numRows << " RamStatus...";
    qry.seek(-1);

    // Parse all data
    QHash<QString,QHash<QString,QJsonObject>> latestItemStepData; // used to check which is the latest status
    QHash<QString,QHash<QString,QString>> latestItemStepUuid; // we need to keep the uuid too
    QStringList statusToMove; // uuids to move from one table to the other
    while (qry.next())
    {
        QString dataStr = qry.value(1).toString();
        // Parse the data
        QJsonDocument doc = QJsonDocument::fromJson( dataStr.toUtf8() );
        QJsonObject obj = doc.object();
        QString itemUuid = obj.value("item").toString();
        if (itemUuid == "") continue;
        QString stepUuid = obj.value("step").toString();
        if (stepUuid == "") continue;

        // Check if we've already found a status for this item/step
        QHash<QString,QJsonObject> stepData = latestItemStepData.value(itemUuid);
        QJsonObject other = stepData.value(stepUuid);

        // Not found yet
        if (other.isEmpty())
        {
            // save data
            stepData.insert(stepUuid, obj);
            latestItemStepData.insert(itemUuid, stepData);
            // save uuid
            QString uuid = qry.value(0).toString();
            QHash<QString,QString> latestStepUuid = latestItemStepUuid.value(itemUuid);
            latestStepUuid.insert(stepUuid, uuid);
            latestItemStepUuid.insert(itemUuid, latestStepUuid);
        }
        else
        {
            // Compare dates
            QDateTime currentDate = QDateTime::fromString( obj.value("date").toString(), DATETIME_DATA_FORMAT );
            QDateTime otherDate = QDateTime::fromString( other.value("date").toString(), DATETIME_DATA_FORMAT );
            if (currentDate > otherDate)
            {
                QHash<QString,QString> latestStepUuid = latestItemStepUuid.value(itemUuid);

                // The other must be moved
                QString otherUuid = latestStepUuid.value(stepUuid);
                statusToMove << otherUuid;

                // save data
                stepData.insert(stepUuid, obj);
                latestItemStepData.insert(itemUuid, stepData);
                // save uuid
                QString uuid = qry.value(0).toString();
                latestStepUuid.insert(stepUuid, uuid);
                latestItemStepUuid.insert(itemUuid, latestStepUuid);
            }
            else
            {
                // This one must be moved
                QString uuid = qry.value(0).toString();
                statusToMove << uuid;
            }
        }
    }

    qDebug() << "Found " << statusToMove.count() << " old status to move in the history";
    qDebug() << "From " << latestItemStepData.count() << " data / " << latestItemStepUuid.count() << " uuids.";

    // Move data to the other table
    if (!statusToMove.isEmpty())
    {
        // Split by 250 rows at a time
        while (!statusToMove.isEmpty())
        {
            QString condition = " `uuid` = '" + statusToMove.takeLast() + "' ";
            for (int i = 0; i < 250; i++)
            {
                if (statusToMove.isEmpty()) break;
                condition += " OR `uuid` = '" + statusToMove.takeLast() + "' ";
            }

            qry.exec("INSERT INTO RamStatusHistory (`uuid`, `data`, `modified`, `removed`) "
                     "SELECT `uuid`, `data`, `modified`, `removed` FROM RamStatus "
                     "WHERE " + condition + " ON CONFLICT(uuid) DO UPDATE SET "
                     "`data` = excluded.data, `modified` = excluded.modified, `removed` = excluded.removed ;"
                     );

            qry.exec("DELETE FROM RamStatus WHERE " + condition);
        }
    }

    // === Vacuum ===

    qry.exec("VACUUM;");

    StateManager::i()->setState(previousState);
}

QSqlQuery LocalDataInterface::query(QString q) const
{
    if (m_dataFile =="") return QSqlQuery();

    QSqlDatabase db = QSqlDatabase::database("localdata");
    QSqlQuery qry = QSqlQuery(db);

    //log(tr("Querying:") + "\n" + q, DuQFLog::Data);
#ifdef DEBUG_DATA
    qDebug() << "<<< SQLITE Query";
    qDebug().noquote() << q;
    qDebug() << ">>>";
#endif

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

void LocalDataInterface::createTable(const QString &tableName)
{
    QString q = "CREATE TABLE IF NOT EXISTS \"%1\" ( "
                "\"id\"	INTEGER NOT NULL UNIQUE, "
                "\"uuid\"	TEXT NOT NULL UNIQUE, "
                "\"data\"	TEXT NOT NULL DEFAULT '{}', "
                "\"modified\"	timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP, "
                "\"removed\"	INTEGER NOT NULL DEFAULT 0, "
                "PRIMARY KEY(\"id\" AUTOINCREMENT) "
                ")";
    query( q.arg( tableName ) );
}

const QHash<QString, QSet<QString> > &LocalDataInterface::deletedUuids() const
{
    return m_uuidsToRemove;
}

