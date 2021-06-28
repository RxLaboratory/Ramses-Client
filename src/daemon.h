#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QTcpServer>
#include <QSettings>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

#include "duqf-app/app-version.h"
#include "duqf-utils/utils.h"
#include "duqf-utils/duqflogger.h"

#include "ramses.h"

class Daemon : public DuQFLoggerObject
{
    Q_OBJECT
public:
    static Daemon *instance();
    void start();
    void stop();
    void restart();

signals:
    void raise();

protected:
    static Daemon *_instance;

private slots:
    void newConnection();
    void reply();
    void ping(QTcpSocket *client);
    void setCurrentProject(QString shortName, QTcpSocket *client);
    void getCurrentStatus(QString shortName, QString name, QString type, QString stepName, QTcpSocket *client);
    void setStatus(QString shortName, QString name, QString step, QString type, QString state, QString comment, int completionRatio, int version, bool published, QString user, QTcpSocket *client);
    void getAssets(QTcpSocket *client);
    void getAsset(QString shortName, QString name, QTcpSocket *client);
    void getAssetGroups(QTcpSocket *client);
    void getProject(QString shortName, QString name, QTcpSocket *client);
    void getCurrentUser(QTcpSocket *client);
    void getPipes(QTcpSocket *client);
    void getProjects(QTcpSocket *client);
    void getShots(QString filter, QTcpSocket *client);
    void getShot(QString shortName, QString name, QTcpSocket *client);
    void getStates(QTcpSocket *client);
    void getState(QString shortName, QString name, QTcpSocket *client);
    void getSteps(QTcpSocket *client);
    void getStep(QString shortName, QString name, QTcpSocket *client);
    void getRamsesFolder(QTcpSocket *client);

private:
    //The daemon is a singleton
    explicit Daemon(QObject *parent = nullptr);
    QTcpServer *_tcpServer;
    QSettings _settings;

    void post(QTcpSocket *client, QJsonObject content, QString query, QString message="", bool success = true, bool accepted = true);

    QJsonObject assetToJson(RamAsset *a);
    QJsonObject shotToJson(RamShot *s);
    QJsonObject stateToJson(RamState *s);
    QJsonObject stepToJson(RamStep *s);
    QJsonObject statusToJson(RamStatus *s);
};

#endif // DAEMON_H
