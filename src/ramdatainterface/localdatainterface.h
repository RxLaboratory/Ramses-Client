#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include <QSqlQuery>
#include <QSqlError>
#include <QStringBuilder>

#include "duqf-utils/duqflogger.h"
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
    static void setRamsesPath(QString dbFile, QString p);
    static QString getRamsesPath(QString dbFile);

    // DATA INTERFACE //

    QSet<QString> tableUuids(QString table, bool includeRemoved = false);
    // Returns a vector instead of set: tabledata may be sorted later
    QVector<QStringList> tableData(QString table, QString filterKey = "", QStringList filterValues = QStringList(), bool includeRemoved = false);
    bool contains(QString uuid, QString table);
    QMap<QString, QString> modificationDates(QString table);

    void createObject(QString uuid, QString table, QString data);

    QString objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QString data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

    void setUsername(QString uuid, QString username);
    bool isUserNameAavailable(const QString &userName);
    void updateUser(QString uuid, QString username, QString data, QString modified);

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
    void setCurrentUserUuid(QString uuid);

    QStringList tableNames();
    QVector<QStringList> users();

    // MAINTENANCE //
    QString cleanDataBase(int deleteDataOlderThan = -1);
    bool undoClean();

    const QHash<QString, QSet<QString> > &deletedUuids() const;

public slots:
    void sync(SyncData data, QString serverUuid = "");

signals:
    void dataReset();
    void ramsesPathChanged(QString);
    // Sync result
    void syncFinished();
    void dataChanged(QString);
    void availabilityChanged(QString,bool);
    void inserted(QString uuid, QString table);
    void removed(QString uuid, QString table);

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

    // Runs a query on the current database
    QSqlQuery query(QString q) const;
    // SQLite vacuum
    void vacuum();

    /**
     * @brief m_dataFile The SQLite file path
     */
    QString m_dataFile;

    // Cache UUIDS to check their existence faster
    QHash<QString, QSet<QString>> m_uuids;

    // The UUIDS to delete when cleaning the database
    QHash<QString, QSet<QString>> m_uuidsToRemove;

};

#endif // LOCALDATAINTERFACE_H
