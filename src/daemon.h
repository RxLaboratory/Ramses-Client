#ifndef DAEMON_H
#define DAEMON_H

#include <QObject>
#include <QTcpServer>
#include <QSettings>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include "duqf-app/app-version.h"
#include "duqf-utils/utils.h"

class Daemon : public QObject
{
    Q_OBJECT
public:
    static Daemon *instance();
    void start();
    void stop();
    void restart();

signals:
    void log(QString, LogUtils::LogType);
    void raise();

protected:
    static Daemon *_instance;

private slots:
    void newConnection();
    void reply();
    void ping(QTcpSocket *client);

private:
    //The daemon is a singleton
    explicit Daemon(QObject *parent = nullptr);
    QTcpServer *_tcpServer;
    QSettings _settings;
};

#endif // DAEMON_H
