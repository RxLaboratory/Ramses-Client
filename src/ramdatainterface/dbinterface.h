#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSettings>
#include <QNetworkCookieJar>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDeadlineTimer>
#include <QApplication>
#include <QStringBuilder>
#include <QTimer>
#include <QThread>
#include <QProgressBar>
#include <QVersionNumber>

#include "duqf-app/app-config.h"
#include "duqf-utils/utils.h"
#include "duqf-utils/duqflogger.h"
#include "ramserverinterface.h"
#include "localdatainterface.h"

class DBInterface : public DuQFLoggerObject
{
    Q_OBJECT
public:
    /**
     * @brief instance Gets the unique DBInterface instance.
     * @return the unique instance.
     */
    static DBInterface *instance();
    /**
     * @brief Generates a hash for a password
     * @param password The password to hash
     * @param salt The salt to use
     * @return The hashed password
     */
    QString generatePassHash(QString password, QString salt = CLIENT_BUILD_KEY);
    /**
     * @brief The current status (offline or online, maybe connecting)
     * @return
     */
    NetworkUtils::NetworkStatus connectionStatus() const;
    /**
     * @brief Changes to offline mode: data is stored locally until we get a connection to the server to sync.
     */
    void setOffline();
    /**
     * @brief Changes to online mode: data is sent to the server and then stored locally.
     * The timestamp is set to be the same.
     * All timestamps must use UTC!
     */
    void setOnline();

    // DATA INTERFACE //

    QJsonObject objectData(QString uuid, QString table);
    void setObjectData(QString uuid, QString table, QJsonObject data);

    void removeObject(QString uuid, QString table);
    void restoreObject(QString uuid, QString table);

    const QString &serverAddress() const;
    void setServerAddress(const QString &newServerAddress);

signals:
    /**
     * @brief Emitted if the mode has changed
     */
    void connectionStatusChanged(NetworkUtils::NetworkStatus, QString);

protected:
    /**
     * @brief The unique DBInteface instance
     */
    static DBInterface *_instance;

private slots:
    /**
     * @brief Handles changes of status from the Ramses server connection
     * @param status
     */
    void serverConnectionStatusChanged(NetworkUtils::NetworkStatus status);
    /**
     * @brief Handles changes of connection status and emits connectionStatusChanged()
     * @param s The new status
     * @param reason A user friendly explanation
     */
    void setConnectionStatus(NetworkUtils::NetworkStatus s, QString reason = "");

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    explicit DBInterface(QObject *parent = nullptr);
    /**
     * @brief Connects all member events (just at the end of constructor method)
     */
    void connectEvents();
    /**
     * @brief The current status (offline, connecting or online)
     */
    NetworkUtils::NetworkStatus m_connectionStatus = NetworkUtils::Offline;
    /**
     * @brief m_rsi is the interface with the Ramses server.
     */
    RamServerInterface *m_rsi;
    /**
     * @brief m_ldi is the interface with the local data.
     */
    LocalDataInterface *m_ldi;
    /**
     * @brief m_serverAddress The address of the server
     */
    QString m_serverAddress;
};

#endif // DBINTERFACE_H
