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

    bool createObject(QString uuid, QString table, QString data);

    QString objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QString data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

    void setUsername(QString uuid, QString username);

    ServerConfig serverConfig() const;

    const QString &dataFile() const;
    ServerConfig setDataFile(const QString &file);

protected:
    static LocalDataInterface *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    LocalDataInterface();
    /**
     * @brief m_dataFile The SQLite file path
     */
    QString m_dataFile;
};

#endif // LOCALDATAINTERFACE_H
