#ifndef LOCALDATAINTERFACE_H
#define LOCALDATAINTERFACE_H

#include "duqf-utils/duqflogger.h"

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

    QJsonObject objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QJsonObject data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);

    void setServerAddress(QString address);

protected:
    static LocalDataInterface *_instance;

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    LocalDataInterface();
    /**
     * @brief The Local database for offline mode (SQLite)
     */
    QSqlDatabase m_localDB;
};

#endif // LOCALDATAINTERFACE_H
