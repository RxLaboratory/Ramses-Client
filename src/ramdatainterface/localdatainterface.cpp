#include "localdatainterface.h"

#include "duapp/dulogger.h"
#include "datacrypto.h"
#include "datastruct.h"
#include "duapp/app-version.h"
#include "duapp/app-config.h"
#include "duutils/utils.h"
#include "statemanager.h"
#include "ramuser.h"
#include "ramses.h"

// INTERFACE

LocalDataInterface *LocalDataInterface::_instance = nullptr;

LocalDataInterface *LocalDataInterface::i()
{
    if (!_instance) _instance = new LocalDataInterface();
    return _instance;
}

void LocalDataInterface::setServerSettings(QString dbFile, ServerConfig c)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << "Can't save data to the disk.";

    // Remove previous settings
    QSqlQuery qry = QSqlQuery(db);
    if (!qry.exec("DELETE FROM _Server;"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" << errorMessage;

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
        qCritical().noquote() << "{Local-Database}" << errorMessage;
    }

    db.close();
}

ServerConfig LocalDataInterface::getServerSettings(QString dbFile)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << tr("Can't save data to the disk.");

    // Get settings
    QSqlQuery qry = QSqlQuery(db);

    if (!qry.exec("SELECT address, useSsl, updateDelay, timeout, port FROM _Server;"))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" << errorMessage;

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

void LocalDataInterface::setWorkingPath(QString dbFile, QString p)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << tr("Can't save data to the disk.");

    QSqlQuery qry = QSqlQuery(db);

    // Add new settings
    QString q = "INSERT INTO _Paths (path, name) "
                "VALUES ('%1', 'Project') "
                "ON CONFLICT(name) DO UPDATE "
                "SET path=excluded.path ;";

    if (!qry.exec(q.arg(p)))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" << errorMessage;
    }

    db.close();
}

QString LocalDataInterface::getWorkingPath(QString dbFile)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    // Set the SQLite file
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << tr("Can't read data.");

    QSqlQuery qry = QSqlQuery( db );
    if (!qry.exec("SELECT path FROM _Paths WHERE name = 'Project';"))
    {
        QString errorMessage = "Something went wrong when reading the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" <<  errorMessage;

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

void LocalDataInterface::setProjectUserUuid(const QString &dbFile, const QString &projectUuid, const QString &userUuid)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << tr("Can't save data to the disk.");

    QSqlQuery qry = QSqlQuery(db);

    // Set everyone to not current
    qry.exec( "UPDATE _User SET current = 0 ;" );

    // Add new settings
    QString q = "INSERT INTO _User (userUuid, projectUuid, current) "
                "VALUES ('%1', '%2', 1) "
                "ON CONFLICT(userUuid) DO UPDATE "
                "SET current=1, projectUuid=excluded.projectUuid ;";

    if (!qry.exec(q.arg(userUuid, projectUuid)))
    {
        QString errorMessage = "Something went wrong when saving the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" << errorMessage;
    }

    db.close();
}

QString LocalDataInterface::projectUuid(const QString &dbFile)
{
    // Make sure the interface is ready
    LocalDataInterface::i();

    QSqlDatabase db = QSqlDatabase::database("editdb");
    if (!openDB(db, dbFile))
        qCritical().noquote() << "{Local-Database}" << tr("Can't read data.");

    QSqlQuery qry = QSqlQuery( db );
    if (!qry.exec("SELECT projectUuid FROM _User WHERE current = 1;"))
    {
        QString errorMessage = "Something went wrong when reading the data.\nHere's some information:";
        errorMessage += "\n> " + tr("Query:") + "\n" + qry.lastQuery();
        errorMessage += "\n> " + tr("Database Error:") + "\n" + qry.lastError().databaseText();
        errorMessage += "\n> " + tr("Driver Error:") + "\n" + qry.lastError().driverText();
        qCritical().noquote() << "{Local-Database}" << errorMessage;

        db.close();
        return "";
    }

    if (qry.first())
    {
        return qry.value(0).toString();
    }

    db.close();

    return "";
}

bool LocalDataInterface::createNewDatabase(const QString &filePath)
{
    // Copy the file
    FileUtils::copy(":/data/template", filePath);

    if (!QFileInfo::exists(filePath))
        return false;

    return true;
}

QSet<QString> LocalDataInterface::tableUuids(QString table, bool includeRemoved)
{
    // If we've got the info in the cache, use it.
    if (includeRemoved && CACHE_LOCAL_DATA && m_uuids.contains(table) )
        return m_uuids.value(table);
    if (!includeRemoved && CACHE_LOCAL_DATA && m_uuidsWithoutRemoved.contains(table) )
        return m_uuidsWithoutRemoved.value(table);

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
                  modified.toString(DATETIME_DATA_FORMAT)
                  )
            );

    emit inserted(uuid, data, modified.toString(DATETIME_DATA_FORMAT), table);
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

void LocalDataInterface::setObjectData(QString uuid, QString table, QString data, bool emitChanged)
{
    // Make sure the table exists
    createTable(table);

    QString newData = DBInterface::i()->validateObjectData(data, uuid, table);

    QDateTime modified = QDateTime::currentDateTimeUtc();

    if (ENCRYPT_USER_DATA && table == "RamUser") newData = DataCrypto::instance()->clientEncrypt(data);
    else newData.replace("'", "''");

    QString q = "INSERT INTO %1 (data, modified, uuid) "
                "VALUES ( '%2', '%3', '%4') "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified ;";

    QString modifiedStr = modified.toString(DATETIME_DATA_FORMAT);
    query( q.arg(table, newData, modifiedStr, uuid) );

    if (emitChanged)
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
    query( q.arg(table, modified.toString(DATETIME_DATA_FORMAT), uuid) );
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
    QString modifiedStr = modified.toString(DATETIME_DATA_FORMAT);
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

void LocalDataInterface::updateUser(const QString &uuid, const QString &role, const QString &data, const QString &modified)
{
    // Adjust modified if not provided
    QString mod = modified;
    if (mod == "")
    {
        QDateTime m = QDateTime::currentDateTimeUtc();
        mod = m.toString(DATETIME_DATA_FORMAT);
    }

    // Encrypt data
    QString newData = data;
    if (ENCRYPT_USER_DATA)
        newData = DataCrypto::instance()->clientEncrypt( newData );
    else
        newData.replace("'", "''");

    // Encrypt role
    QString newRole = role;
    if (ENCRYPT_USER_DATA)
        newRole = DataCrypto::instance()->clientEncrypt( newRole );

    // Insert/update
    QString q = "INSERT INTO RamUser (data, modified, uuid, role, removed) "
                "VALUES ( '%1', '%2', '%3', '%4', 0) "
                "ON CONFLICT(uuid) DO UPDATE "
                "SET data=excluded.data, modified=excluded.modified, role=excluded.role, removed=0 ;";

    query( q.arg(newData, mod, uuid, newRole) );

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

QString LocalDataInterface::workingPath()
{
    QString q = "SELECT path FROM _Paths WHERE name = 'Project';";
    QSqlQuery qry = query( q );
    if (qry.first())
    {
        return qry.value(0).toString();
    }
    return "auto";
}

void LocalDataInterface::setWorkingPath(QString path)
{
    QString q = "INSERT INTO _Paths (path, name) "
                "VALUES ('%1', 'Project') "
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
    changeIOState(Reading);

    QElapsedTimer timer;
    timer.start();

    qDebug() << ">> Opening local file...";

    // Clear all cache
    m_uuids.clear();
    m_uuidsWithoutRemoved.clear();

    qDebug() << ">> Cleared cache: " << timer.elapsed()/1000 << " seconds.";

    QSqlDatabase db = QSqlDatabase::database("localdata");
    openDB(db, file);

    qDebug() << ">> Opened file: " << timer.elapsed()/1000 << " seconds.";

    qInfo().noquote() << tr("Loading data...");

    m_dataFile = file;

    emit dataResetCommon();
    emit dataResetProject();

    qDebug() << ">> Local data ready! " << timer.elapsed()/1000 << " seconds.";

    return serverConfig();
}

SyncData LocalDataInterface::getSync(bool fullSync)
{
    StateChanger s(StateManager::Syncing);

    // List all tables
    QStringList tNames = tableNames();

    // Ignore History for now to improve performance (as it is not used in Ramses yet)
    tNames.removeAll("RamStatusHistory");

    m_fullSync = fullSync;

    // Get last Sync
    QString lastSync = "1818-05-05 00:00:00";
    if (!fullSync)
    {
        QString q = "SELECT lastSync FROM _Sync ;";
        QSqlQuery qry = query(q);
        if (qry.first()) lastSync = qry.value(0).toString();
    }

    // During full sync, clean the data first
    if (fullSync) {
        sanitizeData();
    }

    // For each table, get modified rows

    RamUser *u = Ramses::i()->currentUser();
    QString currentUuid = "";
    if (u) currentUuid = u->uuid();

    QHash<QString, QSet<TableRow>> tables;

    for (int i = 0; i < tNames.count(); i++)
    {
        QString tName = tNames.at(i);

        qInfo().noquote() << tr("Scanning table: %1").arg(tName);

        QSet<TableRow> rows;

        // Make sure the table exists
        createTable(tName);

        QString q;
        q = "SELECT uuid, data, modified, removed FROM %1 ";
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
            if (tName == "RamUser" && ENCRYPT_USER_DATA)
                data = DataCrypto::instance()->clientDecrypt( data );

            row.data = data;
            rows.insert(row);
        }

        tables.insert(tName, rows);
    }

    SyncData syncData;
    syncData.syncDate = lastSync;
    syncData.tables = tables;

    qInfo().noquote() << tr("Successfully scanned local data.");

    return syncData;
}

void LocalDataInterface::saveSync(SyncData syncData)
{
    changeIOState(Writing);

    QHash<QString, QSet<TableRow>> tables = syncData.tables;

    // Insertions
    QHashIterator<QString, QSet<TableRow>> i(tables);
    while (i.hasNext()) {

        i.next();
        QString tableName = i.key();
        if (tableName == "") continue;

        // Make sure the table exists
        createTable(tableName);

        QSet<QStringList> insertedObjects;

        qInfo().noquote() << tr("Inserting new data in: %1").arg(tableName);

        // Clear cache
        m_uuids.remove(tableName);
        m_uuidsWithoutRemoved.remove(tableName);

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // In a single query
        QString q;
        if (tableName == "RamUser") q = "INSERT INTO %1 (data, modified, uuid, removed, role) VALUES ";
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

            QString data = DBInterface::i()->validateObjectData(incomingRow.data, uuid, tableName);

            QString modified = incomingRow.modified;
            int removed = incomingRow.removed;

            if (tableName == "RamUser")
            {
                QString userRole = incomingRow.role;
                if (ENCRYPT_USER_DATA) {
                    data = DataCrypto::instance()->clientEncrypt( data );
                    userRole = DataCrypto::instance()->clientEncrypt( userRole );
                }
                else
                    data.replace("'", "''");

                QString v = "( '%1', '%2', '%3', %4, '%5' )";
                values << v.arg( data, modified, uuid, QString::number(removed), userRole );
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
        changeIOState(Reading);
        foreach(QStringList io, insertedObjects ) {
            emit inserted( io.at(0), io.at(1), io.at(2), io.at(3) );
        }
    }

    changeIOState(Writing);

    // Updates
    i.toFront();
    while (i.hasNext()) {
        i.next();
        QString tableName = i.key();
        if (tableName == "") continue;

        // Make sure the table exists
        createTable(tableName);

        qInfo().noquote() << tr("Updating existing data for: %1").arg(tableName);

        // Clear cache
        m_uuids.remove(tableName);
        m_uuidsWithoutRemoved.remove(tableName);

        QSet<TableRow> incomingRows = i.value();

        // We're going to need the uuids and dates of the table
        QMap<QString, QString> uuidDates = modificationDates( tableName );

        // In a single query
        QString q;
        if (tableName == "RamUser") q = "INSERT INTO %1 (data, modified, uuid, removed, role) VALUES ";
        else q = "INSERT INTO %1 (data, modified, uuid, removed) VALUES ";
        q = q.arg(tableName);

        QStringList values;
        QSet<QStringList> changedUuids;

        foreach(TableRow incomingRow, incomingRows)
        {
            QString uuid = incomingRow.uuid;
            if (uuid == "") continue;
            QString data = DBInterface::i()->validateObjectData(incomingRow.data, uuid, tableName);

            QString modified = incomingRow.modified;
            int rem = incomingRow.removed;
            bool hasBeenRemoved = rem == 1;

            // Only if more recent
            QDateTime incomingDate = QDateTime::fromString(modified, DATETIME_DATA_FORMAT);
            QDateTime currentDate = QDateTime::fromString(uuidDates[uuid], DATETIME_DATA_FORMAT);
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
                QString userRole = incomingRow.role;

                if (ENCRYPT_USER_DATA) {
                    data = DataCrypto::instance()->clientEncrypt( data );
                    userRole = DataCrypto::instance()->clientEncrypt( userRole );
                }
                else data.replace("'", "''");

                QString v = "( '%1', '%2', '%3', %4, '%5' )";
                values << v.arg( data, modified, uuid, QString::number(rem), userRole );
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

        if (tableName == "RamUser") q +="`data` = excluded.data, `modified` = excluded.modified, `removed` = excluded.removed, `role` = excluded.role ;";
        else q += "`data` = excluded.data, `modified` = excluded.modified, `removed` = excluded.removed ;";

        query( q );

        // Emit
        changeIOState(Reading);
        foreach(QStringList cu, changedUuids) {
            emit dataChanged(cu.at(0), cu.at(1), cu.at(2), tableName);
        }
    }

    // Clean if this is a full sync
    if (m_fullSync) {
        backupCurrentDatabase();
        deleteRemovedData();
    }
    vacuum();

    emit syncFinished();
}

void LocalDataInterface::deleteData(SyncData syncData)
{
    QHash<QString, QStringList> tables = syncData.deletedUuids;
    QHashIterator<QString, QStringList> i(tables);

    while (i.hasNext()) {
        i.next();

        QString tableName = i.key();
        QStringList rows = i.value();

        qInfo().noquote() << tr("Removing out-of-date data from: %1").arg(tableName);

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
    QString q = "SELECT userUuid FROM _User WHERE current = 1;";
    QSqlQuery qry = query( q );

    if (qry.first())
        return qry.value(0).toString();
    return "";
}

QString LocalDataInterface::currentProjectUuid()
{
    QString q = "SELECT projectUuid FROM _User WHERE current = 1;";
    QSqlQuery qry = query( q );

    if (qry.first())
        return qry.value(0).toString();
    return "";
}

void LocalDataInterface::setCurrentProjectUser(const QString &projectUuid, const QString &userUuid)
{
    // Set everyone to not current
    query( "UPDATE _User SET current = 0 ;" );

    QString q = "INSERT INTO _User (userUuid, projectUuid, current) "
                "VALUES ('%1', '%2', 1 ) "
                "ON CONFLICT(userUuid) DO UPDATE "
                "SET current=excluded.current ;";

    query( q.arg(userUuid, projectUuid) );
}

QString LocalDataInterface::getUserRole(const QString &uuid)
{
    QString q = "SELECT role FROM RamUser WHERE uuid = '%1';";
    QSqlQuery qry = query( q.arg(uuid) );
    if (qry.first()) {
        QString role = qry.value(0).toString();
        if (ENCRYPT_USER_DATA)
            role = DataCrypto::instance()->clientDecrypt(role);
        return role;
    }
    return "standard";
}

void LocalDataInterface::setUserRole(const QString &uuid, const QString &role)
{
    QString q = "UPDATE RamUser "
                "SET role = '%1' "
                "WHERE uuid = '%2' ;";

    if (ENCRYPT_USER_DATA)
        query( q.arg(DataCrypto::instance()->clientEncrypt(role), uuid)  );
    else
        query( q.arg(role, uuid) );
}

void LocalDataInterface::sync(SyncData data, QString serverUuid)
{
    StateChanger s(StateManager::Syncing);

    qInfo().noquote() << tr("Updating local data...");

    saveSync(data);
    deleteData(data);

    // Save sync date
    QString q = "DELETE FROM _Sync;";
    query( q );
    q = "INSERT INTO _Sync ( lastSync, uuid ) VALUES ( '%1', '%2' );";
    query( q.arg( data.syncDate, serverUuid ) );
}

void LocalDataInterface::sync(SyncData data)
{
    sync(data, "");
}

QStringList LocalDataInterface::tableNames()
{
    changeIOState(Reading);

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
    QString q = "SELECT uuid, data FROM RamUSer ;";
    QSqlQuery qry = query( q );
    QVector<QStringList> us;
    while(qry.next())
    {
        QString data = qry.value(1).toString();
        if (ENCRYPT_USER_DATA) data = DataCrypto::instance()->clientDecrypt( data );
        QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject obj = doc.object();

        QString n = obj.value("name").toString();
        if (n == "")
            n = obj.value("shortName").toString();
        if (n == "")
            continue;

        QStringList u;
        u << qry.value(0).toString() << n;
        us << u;
    }
    return us;
}

QString LocalDataInterface::cleanDataBase(int deleteDataOlderThan)
{
    changeIOState(Writing);

    QString report = "";

    // Backup the DB File
    backupCurrentDatabase();

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

        QString limitDate = QDateTime::currentDateTimeUtc().addDays(-deleteDataOlderThan).toString(DATETIME_DATA_FORMAT);
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

void LocalDataInterface::quit()
{
    // Auto clean!
    autoCleanDB();
    qDebug() << "LocalDataInterface: Vacuuming...";
    vacuum();
    //waitForReady();
    qDebug() << "LocalDataInterface: Everything's clean.";
}

void LocalDataInterface::sanitizeData()
{
    QSignalBlocker b(this);

    qInfo().noquote() << tr("Sanitizing data...");

    qInfo().noquote() << "Cleaning database: sanitizing data...";
    sanitizeStatusTable();
    sanitizeScheduleTable();
}

void LocalDataInterface::sanitizeStatusTable()
{
    qDebug().noquote() << "Sanitizing RamStatus table...";

    // There can be a single status per step and item
    const QSet<QString> stepUuids = tableUuids("RamStep");
    const QSet<QString> itemUuids = tableUuids("RamShot") + tableUuids("RamAsset");

    int count = 0;

    QHash<
        QString, // Step UUID
        QHash<
            QString, // Item UUID
            TableRow // Status
            >
        > sanitizedData;

    const QVector<QStringList> tData = tableData(QStringLiteral("RamStatus"));

    qInfo().noquote() << tr("Sanitizing Statuses...");

    for (const QStringList &row: tData) {

        // This can be long, be nice, process events!
        qApp->processEvents();

        const QString dataStr = row.at(1);
        const QString uuid = row.constFirst();

        QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject dataObj = dataDoc.object();

        // Check step
        QString stepUuid = dataObj.value(
                                      QStringLiteral("step")
                                      ).toString("");
        if (stepUuid == "" ||
            !stepUuids.contains(stepUuid) ) {
            removeObject(uuid, QStringLiteral("RamStatus"));
            ++count;
            continue;
        }

        // Check item
        QString itemUuid = dataObj.value(
                                      QStringLiteral("item")
                                      ).toString("");
        if (itemUuid == "" ||
            !itemUuids.contains(itemUuid) ) {
            removeObject(uuid, QStringLiteral("RamStatus"));
            ++count;
            continue;
        }

        // There can be only one! i.e. the most recent
        QString modified = row.at(2);
        TableRow current = sanitizedData.value(stepUuid).value(itemUuid);
        if (modified >= current.modified) { // More recent

            // Delete older status
            if (current.uuid != "") {
                removeObject(current.uuid, QStringLiteral("RamStatus"));
                ++count;
            }

            // Update current
            current.modified = modified;
            current.uuid = uuid;

            // Insert in sanitized data
            QHash<QString,TableRow> byItem = sanitizedData.value(stepUuid);
            byItem.insert(itemUuid, current);
            sanitizedData.insert(stepUuid, byItem);
        }
        else { // Older
            // Just remove
            removeObject(uuid, QStringLiteral("RamStatus"));
            ++count;
        }
    }

    qDebug().noquote() << "Removed" << count << "obsolete status.";
}

void LocalDataInterface::sanitizeScheduleTable()
{
    // Entries must have an existing row

    const QSet<QString> rowUuids = tableUuids("RamScheduleRow");

    int count = 0;

    const QVector<QStringList> tData = tableData(QStringLiteral("RamScheduleEntry"));

    qInfo().noquote() << tr("Sanitizing Schedule...");

    for (const QStringList &row: tData) {

        // This can be long, be nice, process events!
        qApp->processEvents();

        const QString dataStr = row.at(1);
        const QString uuid = row.constFirst();

        QJsonDocument dataDoc = QJsonDocument::fromJson(dataStr.toUtf8());
        QJsonObject dataObj = dataDoc.object();

        // check row
        QString rowUuid = dataObj.value(
                                     QStringLiteral("row")
                                     ).toString("");
        if (rowUuid == "" ||
            !rowUuids.contains(rowUuid) ) {
            removeObject(uuid, QStringLiteral("RamScheduleEntry"));
            ++count;
            continue;
        }
    }

    qDebug().noquote() << "Removed" << count << "obsolete schedule entries.";
}

void LocalDataInterface::deleteRemovedData()
{
    qInfo().noquote() << "Cleaning database: deleting obsolete data...";
    const QStringList tNames = tableNames();
    for(const QString &tName: tNames) {
        // This can be long, be nice, process events!
        qApp->processEvents();

        deleteRemovedData(tName);
    }
}

void LocalDataInterface::deleteRemovedData(const QString &tableName)
{
    qDebug().noquote() << "Deleting obsolete data for table" << tableName;
    QString q = "DELETE FROM `%1` WHERE `removed` = 1;";
    query( q.arg(tableName) );
}

void LocalDataInterface::backupCurrentDatabase()
{
    qInfo().noquote() << tr("Backuping current database...");

    QFileInfo dbFileInfo(m_dataFile);
    QString backupFile = dbFileInfo.path() + "/" + dbFileInfo.baseName() + "_bak." + dbFileInfo.completeSuffix();
    if (QFileInfo::exists(backupFile)) FileUtils::remove(backupFile);
    FileUtils::copy(m_dataFile, backupFile);
}

LocalDataInterface::LocalDataInterface():
    QObject()
{
    DuLogger::i()->registerComponent("Local Database", this);

    //Load local database
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","localdata");
    db.setHostName("localhost");

    QSqlDatabase editdb = QSqlDatabase::addDatabase("QSQLITE","editdb");
    editdb.setHostName("localhost");
    QSqlDatabase infodb = QSqlDatabase::addDatabase("QSQLITE","infodb");
    infodb.setHostName("localhost");

    //m_queryThread.start();

    _ioTimer.setSingleShot(true);

    connect(qApp, &QApplication::aboutToQuit, this, &LocalDataInterface::quit);
    connect(&_ioTimer, &QTimer::timeout, this, [this] {

        emit stateChanged(_waitingIOSignal);

        if (_waitingIOSignal != Idle) {
            _waitingIOSignal = Idle;
            // Restart the timer to emit Idle in a few moments
            _ioTimer.start(_minIOInterval);
        }

    });
}

bool LocalDataInterface::openDB(QSqlDatabase db, const QString &dbFile)
{
    qInfo().noquote() << tr("Opening database...");

    // Make sure the DB is closed
    db.close();
    // Open
    db.setDatabaseName(dbFile);
    if (!db.open()) {
        qCritical().noquote() << "{Local-Database}" << tr("Can't save data to the disk.");
        return false;
    }

    // Check the version, and update the db scheme
    qDebug().noquote() << tr("Checking database version");

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
        qDebug().noquote() << tr("Updating database scheme");
        QFileInfo dbFileInfo(dbFile);
        qInfo().noquote() << "{Local-Database}" << tr("This database was created by an older version of Ramses (%1).\n"
                                                          "I'm updating it to the current version (%2).\n"
                                                          "The original file will be renamed to \"%3_%1.ramses\".").arg(currentVersion.toString(), STR_VERSION, dbFileInfo.baseName());

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
                qCritical().noquote() << "{Local-Database}" << errorMessage;
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
                qCritical().noquote() << "{Local-Database}" << errorMessage;
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
                qCritical().noquote() << "{Local-Database}" << errorMessage;
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
                qWarning().noquote() << "{Local-Database}" << errorMessage;
            }
        }
    }
    else if (currentVersion > newVersion)
    {
        qCritical().noquote() << "{Local-Database}" << tr("This database was created by a more recent version of Ramses (%1).\n"
                                                          "You should update this Ramses application before using this database!\n"
                                                          "Be careful if you continue, this could lead to data loss or corrupted databases.").arg(currentVersion.toString());
        ok = false;
    }

    // If not ok, finished
    if (!ok) return true;

    return true;
}

void LocalDataInterface::autoCleanDB()
{
    qInfo().noquote() << "Cleaning the database...";

    auto db = QSqlDatabase::database("localdata");

    QSqlQuery qry = QSqlQuery(db);

    if ( !db.isOpen() )
    {
        if ( !db.open() ) return;
    }

    // === Delete removed statuses ===

    qry.exec("DELETE FROM RamStatus WHERE `removed` = 1 ;");

    // === Delete removed schedule entries ===

    qry.exec("DELETE FROM RamScheduleEntry WHERE `removed` = 1 ;");
}

QSqlQuery LocalDataInterface::query(QString q)
{
    if (m_dataFile =="") return QSqlQuery();

    State state = Writing;
    if (q.startsWith("SELECT"))
        state = Reading;

    changeIOState(state);

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
        qCritical().noquote() << "{Local-Database}" << errorMessage;
    }

    return qry;
    //return m_querier->query(q);
}

void LocalDataInterface::vacuum()
{
    changeIOState(Writing);
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

void LocalDataInterface::changeIOState(State s)
{
    // If we're idle, just restart the timer
    if (_waitingIOSignal == Idle) {
        _ioTimer.start(_minIOInterval);
        _waitingIOSignal = s;
        return;
    }

    if (_waitingIOSignal == Reading &&
             s == Writing)
        _waitingIOSignal = ReadWrite;

    else if (_waitingIOSignal == Writing &&
             s == Reading)
        _waitingIOSignal = ReadWrite;
}

const QHash<QString, QSet<QString> > &LocalDataInterface::deletedUuids() const
{
    return m_uuidsToRemove;
}

