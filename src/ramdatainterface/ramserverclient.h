#ifndef RAMSERVERCLIENT_H
#define RAMSERVERCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QNetworkReply>
#include <QJsonObject>

#include "datastruct.h"

class RamServerClient: public QObject
{
    Q_OBJECT

public:

    // ==== STATIC ====

    // Instance

    static RamServerClient *i();

    // ==== ENUMS ====

    enum ClientStatus {
        Offline, // Needs a ping before continuing
        Online, // Ping successful, the server is working but the user is not set
        Ready, // User authenticated, idle and can sync
        Syncing, // Currently syncing
    };

    // ==== STATUS AND SETTINGS ====

    ServerConfig serverConfig() const;

    ClientStatus status() const { return m_status; }
    QString lastError() const { return m_lastError; }

    // Server Settings

    void setServerSettings(ServerConfig settings);

    QString serverAddress() const { return m_serverAddress; }
    void setServerAddress(const QString &newServerAddress);

    int serverPort() const { return m_serverPort; }
    void setServerPort(int newServerPort) {  m_serverPort = newServerPort; }

    bool ssl() const { return m_ssl; }
    void setSsl(bool useSsl);
    QString serverProtocol() const;

    int timeOut() const { return m_timeout; }
    void setTimeout(int newTimeout) { m_timeout = newTimeout; }

    void setOffline();

    // ==== RAMSES API ====

    // Synchronous methods
    QJsonObject ping(bool synchronous);
    QJsonObject login(const QString &email, const QString &password);
    QJsonObject setUserRole(const QString &uuid, const QString &role);
    QJsonObject createProject(const QString &data, const QString &uuid = "");
    QJsonObject setProject(const QString &uuid);
    QJsonObject assignUsers(const QStringList &userUuids, const QString &projectUuid);
    QJsonObject getAllUsers();
    QJsonObject setPassword(const QString &userUuid, const QString &newPassword, const QString &currentPassword = "");
    QJsonObject getEmail(const QString &uuid);
    QJsonObject setEmail(const QString &uuid, const QString &email);
    QJsonObject createUsers(const QJsonArray &users );
    QJsonObject getProjects();
    // QJsonObject unassignUser(const QString &userUuid, const QString &projectUuid);
    // QJsonObject getUsers(const QString &projectUuid = "");
    //
    // QJsonObject deleteData(QHash<QString, QSet<QString> > uuidsToDelete);

    // ASync methods
    void sync(SyncData syncData);
    void downloadAllData();

signals:
    void sslChanged(bool);
    void statusChanged(ClientStatus status);
    void syncReady(const SyncData &data);

private:
    // Singleton
    static RamServerClient *_instance;
    RamServerClient();

    // ====== METHODS ======

    // Networking

    void nextRequest();
    void dataReceived(QNetworkReply *reply);
    void sslError(QNetworkReply *reply, QList<QSslError> errs);
    void flushRequests();
    void networkError(QNetworkReply::NetworkError err);

    void startQueue();
    void pauseQueue();
    void queueRequest(QString query, QJsonObject body = QJsonObject());
    void queueRequest(Request r);

    Request buildRequest(QString query, QJsonObject body);
    Request buildRequest(QString query);

    void postRequest(Request r);
    QNetworkReply *synchronousRequest(Request r);

    QJsonObject parseData(QNetworkReply *reply);

    // Sync Process

    void startSync();
    void fetch();
    void push(QString table, QSet<TableRow> rows = QSet<TableRow>(), QString date = "1818-05-05 00:00:00", bool commit = false);
    void pull(QString table, int page = 1);
    void pushNext();
    void pullNext();
    void commit();
    void finishSync(bool inError = false);

    // Other Ramses API Helpers

    QString hashPassword(const QString &password);

    // Status

    void setStatus(ClientStatus status);

    // ====== ATTRIBUTES ======

    // Status and settings

    ClientStatus m_status = Offline;
    QString m_lastError;

    QString m_sessionToken = "";

    QString m_serverAddress;
    int m_serverPort = 443;
    bool m_ssl = false;

    // Network

    QNetworkAccessManager *m_network; // connected for asynchronous queries
    QNetworkAccessManager *m_synchronousNetwork; // used for synchronous queries

    QStringList m_forbiddenWords; // A list of forbidden words in URLS

    QVector<Request> m_requestQueue; // Requests waiting to be sent
    QTimer *m_requestQueueTimer; // Timer to send requests at regular intervals
    int m_requestDelay = 250; // Delay between requests in ms. Hardcoded for now
    int m_timeout = 3000; // Server timeout
    int m_requestMaxRows = 1000; // Max request size

    // Sync process

    SyncData m_pulledData;
    SyncData m_pushingData;
    FetchData m_fetchData;
};

#endif // RAMSERVERCLIENT_H
