#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include <QSqlQuery>

#include "duqf-utils/duqflogger.h"

struct ServerConfig {
    QString address = "";
    int updateDelay = 60;
    int timeout = 3;
    bool useSsl = true;
    QString path = "";
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

    // DATA INTERFACE //

    void createObject(QString uuid, QString table, QJsonObject data);

    QJsonObject objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QJsonObject data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);
    bool isRemoved(QString uuid, QString table);

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
