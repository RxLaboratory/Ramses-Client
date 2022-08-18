#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include <QSqlQuery>
#include <QSqlError>
#include <QStringBuilder>

#include "duqf-utils/duqflogger.h"

struct ServerConfig {
    QString address = "";
    int updateDelay = 60;
    int timeout = 3;
    bool useSsl = true;
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
    QString login(QString username, QString password) const;

    /**
     * @brief setRamsesPath sets the path to the local data for this database
     * @param p
     */
    void setRamsesPath(QString p) const;

    // DATA INTERFACE //

    QStringList tableData(QString table) const;
    bool contains(QString uuid, QString table) const;

    void createObject(QString uuid, QString table, QString data) const;

    QString objectData(QString uuid, QString table) const;
    void setObjectData(QString uuid, QString table, QString data) const;

    void removeObject(QString uuid, QString table) const;
    void restoreObject(QString uuid, QString table) const;
    bool isRemoved(QString uuid, QString table) const;

    void setUsername(QString uuid, QString username) const;

    ServerConfig serverConfig() const;

    const QString &dataFile() const;
    ServerConfig setDataFile(const QString &file);

signals:
    void dataReset();

protected:
    static LocalDataInterface *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    LocalDataInterface();

    QSqlQuery query(QString q) const;

    /**
     * @brief m_dataFile The SQLite file path
     */
    QString m_dataFile;
};

#endif // LOCALDATAINTERFACE_H
