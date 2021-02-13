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

#include "duqf-utils/utils.h"
#include "duqf-app/app-version.h"

class DBInterface : public QObject
{
    Q_OBJECT
public:
    static DBInterface *instance();
    //connection
    void login(QString username, QString password);
    //users
    void getUsers();

signals:
    void log(QString, LogUtils::LogType);
    void statusChanged(NetworkUtils::NetworkStatus, QString);
    void data(QJsonObject);

protected:
    static DBInterface *_instance;

private slots:
    void dataReceived(QNetworkReply *rep);
    void sslError(QNetworkReply *rep, QList<QSslError> errs);
    void networkError(QNetworkReply::NetworkError err);
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

    QNetworkAccessManager _network;
    QNetworkReply *_reply;

    NetworkUtils::NetworkStatus _status;

    void request(QString req, QJsonDocument content = QJsonDocument());
    QString generatePassHash(QString password, QString salt = "salt");
};

#endif // DBINTERFACE_H
