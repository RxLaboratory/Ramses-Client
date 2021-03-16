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
#include "duqf-utils/duqflogger.h"

class DBInterface : public DuQFLoggerObject
{
    Q_OBJECT
public:
    static DBInterface *instance();
    //connection
    NetworkUtils::NetworkStatus connectionStatus() const;
    void setOffline();
    void setOnline();
    bool isSuspended() const;
    //users
    void login(QString username, QString password);
    void getUsers();
    void updateUser(QString uuid, QString shortName, QString name, QString role = "", QString folderPath = "");
    void updateUserPassword(QString uuid, QString c, QString n);
    void createUser(QString shortName, QString name, QString uuid, QString password = "password");
    void removeUser(QString uuid);
    //projects
    void createProject(QString shortName, QString name, QString uuid);
    void getProjects();
    void getProject(QString uuid);
    void updateProject(QString uuid, QString shortName, QString name, QString folderPath = "");
    void removeProject(QString uuid);
    void assignStep(QString stepUuid, QString projectUuid);
    //template steps
    void createTemplateStep(QString shortName, QString name, QString uuid);
    void getTemplateSteps();
    void updateTemplateStep(QString uuid, QString shortName, QString name, QString type = "");
    void removeTemplateStep(QString uuid);
    //template asset groups
    void createTemplateAssetGroup(QString shortName, QString name, QString uuid);
    void getTemplateAssetGroups();
    void updateTemplateAssetGroup(QString uuid, QString shortName, QString name);
    void removeTemplateAssetGroup(QString uuid);
    //steps
    void createStep(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateStep(QString uuid, QString shortName, QString name, QString type, int order);
    void removeStep(QString uuid);
    void assignUser(QString stepUuid, QString userUuid);
    void unassignUser(QString stepUuid, QString userUuid);
    //asset groups
    void createAssetGroup(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateAssetGroup(QString uuid, QString shortName, QString name);
    void removeAssetGroup(QString uuid);
    //assets
    void createAsset(QString shortName, QString name, QString assetGroupUuid, QString tags, QString uuid);
    void updateAsset(QString uuid, QString shortName, QString name, QString assetGroupUuid, QString tags);
    void removeAsset(QString uuid);
    //sequences
    void createSequence(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateSequence(QString uuid, QString shortName, QString name);
    void removeSequence(QString uuid);
    //states
    void createState(QString shortName, QString name, QString uuid);
    void getStates();
    void updateState(QString uuid, QString shortName, QString name, QString color = "", QString completionRatio = "");
    void removeState(QString uuid);
    //File types
    void createFileType(QString shortName, QString name, QStringList extensions, QString uuid);
    void getFileTypes();
    void updateFileType(QString uuid, QString shortName, QString name, QStringList extensions);
    void removeFileType(QString uuid);

public slots:
    void suspend(bool suspended = true);

signals:
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
     * @brief True to suspend the interface, used when creating Ramses objects from a database update
     */
    bool _suspended;
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
