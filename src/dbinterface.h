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
    void init();
    //users
    void login(QString username, QString password);
    void getUsers();
    void updateUser(QString uuid, QString shortName, QString name, QString role = "", QString folderPath = "", QString comment = "");
    void updateUserPassword(QString uuid, QString c, QString n);
    void createUser(QString shortName, QString name, QString uuid, QString password = "password");
    void removeUser(QString uuid);
    //projects
    void createProject(QString shortName, QString name, QString uuid);
    void getProjects();
    void getProject(QString uuid);
    void updateProject(QString uuid, QString shortName, QString name, int width = 0, int height = 0, double framerate = 0.0, QString folderPath = "", QString comment = "", QDate deadline = QDate());
    void removeProject(QString uuid);
    void assignStep(QString stepUuid, QString projectUuid);
    //template steps
    void createTemplateStep(QString shortName, QString name, QString uuid);
    void getTemplateSteps();
    void updateTemplateStep(QString uuid, QString shortName, QString name, QString type = "", QString comment = "",  QColor color = QColor());
    void removeTemplateStep(QString uuid);
    void setTemplateStepEstimations(QString uuid, QString method, float veryEasy = 0.2, float easy = 0.5, float medium = 1.0, float hard = 2.0, float veryHard = 3.0);
    //template asset groups
    void createTemplateAssetGroup(QString shortName, QString name, QString uuid);
    void getTemplateAssetGroups();
    void updateTemplateAssetGroup(QString uuid, QString shortName, QString name, QString comment = "" );
    void removeTemplateAssetGroup(QString uuid);
    //steps
    void createStep(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateStep(QString uuid, QString shortName, QString name, QString type, QString comment = "", QColor color = QColor());
    void moveStep(QString uuid, int order);
    void setStepOrder(QString uuid, int order);
    void setStepEstimations(QString uuid, QString method, float veryEasy = 0.2, float easy = 0.5, float medium = 1.0, float hard = 2.0, float veryHard = 3.0, QString multiplyGroupUuid = "");
    void removeStep(QString uuid);
    void assignUser(QString stepUuid, QString userUuid);
    void unassignUser(QString stepUuid, QString userUuid);
    void assignApplication(QString stepUuid, QString applicationUuid);
    void unassignApplication(QString stepUuid, QString applicationUuid);
    //asset groups
    void createAssetGroup(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateAssetGroup(QString uuid, QString shortName, QString name, QString comment = "");
    void removeAssetGroup(QString uuid);
    //assets
    void createAsset(QString shortName, QString name, QString assetGroupUuid, QString tags, QString uuid);
    void updateAsset(QString uuid, QString shortName, QString name, QString assetGroupUuid, QString tags, QString comment = "");
    void removeAsset(QString uuid);
    void setAssetStatus(QString assetUuid, QString stateUuid, QString stepUuid, QString userUuid = "", int completionRatio = -1, QString comment = "", int version = 1, QString uuid = "", QString assignedUserUuid = "");
    //sequences
    void createSequence(QString shortName, QString name, QString projectUuid, QString uuid);
    void updateSequence(QString uuid, QString shortName, QString name, QString comment = "");
    void removeSequence(QString uuid);
    void setSequenceOrder(QString uuid, int order);
    //shots
    void createShot(QString shortName, QString name, QString sequenceUuid, QString uuid, qreal duration = 0.0, int order = -1);
    void updateShot(QString uuid, QString shortName, QString name, QString sequenceUuid = "", qreal duration = -1.0, QString comment = "");
    void moveShot(QString uuid, int order);
    void setShotOrder(QString uuid, int order);
    void removeShot(QString uuid);
    void setShotStatus(QString shotUuid, QString stateUuid, QString stepUuid, QString userUuid = "", int completionRatio = -1, QString comment = "", int version = 1, QString uuid = "", QString assignedUserUuid = "");
    void assignAsset(QString shotUuid, QString assetUuid);
    void unassignAsset(QString shotUuid, QString assetUuid);
    //states
    void createState(QString shortName, QString name, QString uuid);
    void getStates();
    void updateState(QString uuid, QString shortName, QString name, QString color = "", QString completionRatio = "",  QString comment = "");
    void removeState(QString uuid);
    //File types
    void createFileType(QString shortName, QString name, QStringList extensions, QString uuid);
    void getFileTypes();
    void updateFileType(QString uuid, QString shortName, QString name, QStringList extensions, bool previewable = false, QString comment = "");
    void removeFileType(QString uuid);
    //Applications
    void createApplication(QString shortName, QString name, QString executableFilePath, QString uuid);
    void getApplications();
    void updateApplication(QString uuid, QString shortName, QString name, QString executableFilePath, QString comment = "");
    void removeApplication(QString uuid);
    void assignFileType(QString applicationUuid, QString fileTypeUuid, QString type = "");
    void unassignFileType(QString applicationUuid, QString fileTypeUuid, QString type = "");
    //Pipes
    void createPipe(QString outputUuid, QString inputUuid, QString uuid);
    void updatePipe(QString uuid, QString inputUuid, QString outputUuid);
    void removePipe(QString uuid);
    void assignPipeFile(QString pipeUuid, QString pipeFileUuid);
    void unassignPipeFile(QString pipeUuid, QString pipeFileUuid);
    //PipeFiles
    void createPipeFile(QString shortName, QString projectUuid, QString fileTypeUuid, QString uuid, QString colorSpaceUuid = "");
    void updatePipeFile(QString uuid, QString shortName, QString fileTypeUuid = "", QString colorSpaceUuid = "", QString comment = "" );
    void removePipeFile(QString uuid);
    //Status
    void updateStatus(QString uuid, QString stateUuid, QString comment = "", int version = 0, int completionRatio = -1, bool published = false, QString assignedUserUuid = "", qint64 timeSpent = -1, QDateTime date = QDateTime(), QString difficulty = "medium", float estimation = -1);
    void setStatusUser(QString uuid, QString userUuid);
    void removeStatus(QString uuid);
    // Schedule
    void createSchedule(QString userUuid, QString stepUuid, QDateTime date, QString uuid = "");
    void updateSchedule(QString uuid, QString userUuid, QString stepUuid, QDateTime date, QString comment);
    void removeSchedule(QString uuid);

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
     * @brief _forbiddenWords A list of forbidden words in URLs
     */
    QStringList _forbiddenWords;

    /**
     * @brief Requests data from the remote server
     * @param req The request to post
     * @param content
     */
    void request(QString query, QStringList args = QStringList(), bool wait = true);
    QString buildFormEncodedString(QStringList args);
    bool waitPing();
    QString getServerAddress();
    QString getProtocol();

    /**
     * @brief Generates a hash for a password
     * @param password The password to hash
     * @param salt The salt to use
     * @return The hashed password
     */
    QString generatePassHash(QString password, QString salt = "H6BuYLsW");
};

#endif // DBINTERFACE_H
