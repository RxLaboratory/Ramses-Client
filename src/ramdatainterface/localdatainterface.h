#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include <QSqlQuery>
#include <QSqlError>
#include <QStringBuilder>

#include "duutils/duqflogger.h"
#include "datastruct.h"

class LocalDataInterface : public DuQFLoggerObject
{
    Q_OBJECT

public:
    /**
     * @brief instance returns the unique instance of RamServerInterface.
     * @return the instance.
     */
    static LocalDataInterface *instance();

    /**
     * @brief setServerSettings updates the server settings for a given database
     * @param dbFile The database to update
     * @param c The new settings
     */
    static void setServerSettings(QString dbFile, ServerConfig c);
    static ServerConfig getServerSettings(QString dbFile);
    static void setWorkingPath(QString dbFile, QString p);
    static QString getWorkingPath(QString dbFile);
    static void setProjectUserUuid(const QString &dbFile, const QString &projectUuid, const QString &userUuid);
    static QString projectUuid(const QString &dbFile);
    static bool createNewDatabase(const QString &filePath);

    // DATA INTERFACE //

    QSet<QString> tableUuids(QString table, bool includeRemoved = false);
    // Returns a vector instead of set: tabledata may be sorted later
    QVector<QStringList> tableData(QString table, QHash<QString, QStringList> filters = QHash<QString, QStringList>(), bool includeRemoved = false);
    bool contains(QString uuid, QString table, bool includeRemoved = false);
    QMap<QString, QString> modificationDates(QString table);

    void createObject(QString uuid, QString table, QString data);

    QString objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QString data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

    QString modificationDate(QString uuid, QString table);

    void updateUser(const QString &uuid, const QString &role, const QString &data, const QString &modified);

    ServerConfig serverConfig();
    void setServerUuid(QString serverUuid);
    QString serverUuid();

    QString ramsesPath();
    void setRamsesPath(QString path);

    const QString &dataFile() const;
    ServerConfig setDataFile(const QString &file);

    SyncData getSync(bool fullSync=true);
    void saveSync(SyncData syncData);
    void deleteData(SyncData syncData);

    QString currentUserUuid();
    QString currentProjectUuid();
    void setCurrentProjectUser(const QString &projectUuid, const QString &userUuid);

    QString getUserRole(const QString &uuid);
    void setUserRole(const QString &uuid, const QString &role);

    QStringList tableNames();
    QVector<QStringList> users();

    // MAINTENANCE //
    QString cleanDataBase(int deleteDataOlderThan = -1);
    bool undoClean();

    const QHash<QString, QSet<QString> > &deletedUuids() const;

public slots:
    void sync(SyncData data, QString serverUuid);
    void sync(SyncData data);

signals:
    void dataResetCommon();
    void dataResetProject();
    void ramsesPathChanged(QString);
    // Sync result
    void syncFinished();
    void dataChanged(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table);
    void availabilityChanged(QString,bool);
    void inserted(const QString &uuid, const QString &data, const QString &modificationDate, const QString &table);
    void removed(const QString &uuid, const QString &table);

protected:
    static LocalDataInterface *_instance;

private slots:
    void logError(QString err);
    void quit();

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    LocalDataInterface();

    // Opens the database, updates the scheme if needed
    static bool openDB(QSqlDatabase db, const QString &dbFile);

    static void autoCleanDB(QSqlDatabase db);

    // Runs a query on the current database
    QSqlQuery query(QString q) const;
    // SQLite vacuum
    void vacuum();

    // Makes sure a table exists
    // Creates if not exists
    void createTable(const QString &tableName);

    /**
     * @brief m_dataFile The SQLite file path
     */
    QString m_dataFile;

    // Cache UUIDS to check their existence faster
    QHash<QString, QSet<QString>> m_uuids;
    QHash<QString, QSet<QString>> m_uuidsWithoutRemoved;

    // The UUIDS to delete when cleaning the database
    QHash<QString, QSet<QString>> m_uuidsToRemove;

};

#endif // LOCALDATAINTERFACE_H
