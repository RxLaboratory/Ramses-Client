#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include <QSqlQuery>
#include <QSqlError>
#include <QStringBuilder>

#include "duqf-utils/duqflogger.h"

struct ServerConfig {
    QString address = "";
    int updateDelay = 60000;
    int timeout = 3000;
    bool useSsl = true;
};

class Querier : public DuQFLoggerObject
{
    Q_OBJECT
public:
    Querier(QString dbName);
public slots:
    void setDataFile(QString f);
    QSqlQuery query(QString q);
signals:
    void queryFinished(QString q);
    void ready(QSqlQuery);
    void error(QString);
private:
    QString m_query;
    QString m_dataFile;
    QString m_dbName;
};

class LocalDataInterface : public DuQFLoggerObject
{
    Q_OBJECT

public:
    /**
     * @brief instance returns the unique instance of RamServerInterface.
     * @return the instance.
     */
    static LocalDataInterface *instance();

    bool isReady() const;
    void waitForReady(int timeout=5000) const;

    /**
     * @brief setServerSettings updates the server settings for a given database
     * @param dbFile The database to update
     * @param c The new settings
     */
    static void setServerSettings(QString dbFile, ServerConfig c);
    static ServerConfig getServerSettings(QString dbFile);

    // AUTHENTIFICATION //

    /**
     * @brief login Checks the username and password in the local data
     * @param username
     * @param password
     * @return The user uuid if successful, empty string otherwise
     */
    QString login(QString username, QString password);

    /**
     * @brief setRamsesPath sets the path to the local data for this database
     * @param p
     */
    void setRamsesPath(QString p);

    // DATA INTERFACE //

    QStringList tableData(QString table);
    bool contains(QString uuid, QString table);

    void createObject(QString uuid, QString table, QString data);

    QString objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QString data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

    void setUsername(QString uuid, QString username);

    ServerConfig serverConfig();

    const QString &dataFile() const;
    ServerConfig setDataFile(const QString &file);

    QJsonObject getSync();
    void saveSync(QJsonArray tables);

    QString currentUserUuid();
    void setCurrentUserUuid(QString uuid);

    QStringList tableNames();

public slots:
    void sync(QJsonArray tables);

signals:
    void dataReset();
    void newQuery(QString);
    void newDataFile(QString);
    // emitted when all save queries have terminated
    // (everything's been written and it's safe to get data again)
    void ready();
    // Sync trigger
    void readyToSync(QJsonArray tables, QString lastSyncDate);
    // Sync result
    void dataChanged(QString);
    void availabilityChanged(QString,bool);
    void inserted(QString,QString);

protected:
    static LocalDataInterface *_instance;

private slots:
    void logError(QString err);
    void finishQuery(QString q);
    void quit();
    void processUpdates();

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    LocalDataInterface();

    QSqlQuery query(QString q);
    void threadedQuery(QString q);
    void vacuum();

    /**
     * @brief m_dataFile The SQLite file path
     */
    QString m_dataFile;
    QThread m_queryThread;
    Querier *m_tQuerier;
    Querier *m_querier;
    QStringList m_activeQueries;

    /**
     * Changes to emit after syncing
     */
    QList<QStringList> m_inserted;
    QStringList m_updated;
    QMap<QString, bool> m_availabilityChanged;
};

#endif // LOCALDATAINTERFACE_H
