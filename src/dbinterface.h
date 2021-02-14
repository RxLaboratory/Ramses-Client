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

#include "duqf-utils/utils.h"
#include "duqf-app/app-version.h"

class DBInterface : public QObject
{
    Q_OBJECT
public:
    static DBInterface *instance();
    //connection
    NetworkUtils::NetworkStatus connectionStatus() const;
    void setOffline();
    void setOnline();
    //users
    void login(QString username, QString password);
    void getUsers();
    void updateUser(QString uuid, QString shortName, QString name);
    void updateUserPassword(QString uuid, QString c, QString n);
    void createUser(QString shortName, QString name, QString uuid, QString password = "password");
    void removeUser(QString uuid);
signals:
    void log(QString, LogUtils::LogType);
    void connectionStatusChanged(NetworkUtils::NetworkStatus);
    void data(QJsonObject);

protected:
    static DBInterface *_instance;

private slots:
    void dataReceived(QNetworkReply *rep);
    void sslError(QNetworkReply *rep, QList<QSslError> errs);
    void networkError(QNetworkReply::NetworkError err);
    void setConnectionStatus(NetworkUtils::NetworkStatus s);

private:
    /**
     * @brief This is a singleton, private constructor
     * @param parent
     */
    explicit DBInterface(QObject *parent = nullptr);

    /**
     * @brief A Local database for offline mode
     */
    QSqlDatabase _localDB;
    /**
     * @brief Manages the remote connection
     */
    QNetworkAccessManager _network;
    /**
     * @brief Stores the replies from the remote connection
     */
    QNetworkReply *_reply;

    /**
     * @brief Online / Offline status
     */
    NetworkUtils::NetworkStatus _status;
    /**
     * @brief The token given by the server when logging in.
     */
    QString _sessionToken;

    /**
     * @brief Requests data from the remote server
     * @param req The request to post
     * @param content
     */
    void request(QString req, bool waitPing = true);
    void request(QStringList args);

    /**
     * @brief Generates a hash for a password
     * @param password The password to hash
     * @param salt The salt to use
     * @return The hashed password
     */
    QString generatePassHash(QString password, QString salt = "H6BuYLsW");
};

#endif // DBINTERFACE_H
