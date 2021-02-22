#include "daemon.h"

Daemon *Daemon::_instance = nullptr;

Daemon* Daemon::instance()
{
    if (!_instance) _instance = new Daemon();
    return _instance;
}

void Daemon::start()
{
    if (!_tcpServer->listen( QHostAddress::LocalHost, _settings.value("daemon/port", 18185).toInt() )) {
        qDebug() << _tcpServer->errorString();
        emit log("Unable to start the daemon server.\n" + _tcpServer->errorString(), LogUtils::Warning);
    }
    else
    {
        qDebug() << "Daemon started and listening on port " + QString::number(_tcpServer->serverPort());
        emit log("Daemon started and listening on port " + QString::number(_tcpServer->serverPort()), LogUtils::Information);
    }
}

void Daemon::stop()
{
    _tcpServer->close();
    emit log("Daemon stopped.", LogUtils::Information);
}

void Daemon::restart()
{
    stop();
    start();
}

void Daemon::newConnection()
{
    QTcpSocket *client = _tcpServer->nextPendingConnection();
    connect(client, &QAbstractSocket::disconnected, client, &QObject::deleteLater);
    connect(client, &QAbstractSocket::readyRead, this, &Daemon::reply);
}

void Daemon::reply()
{
    QTcpSocket *client = (QTcpSocket*)sender();
    QString request = client->readAll();
    //split args
    QStringList requestArgs = request.split("&");
    log("Got args: \n" + requestArgs.join("\n"), LogUtils::Debug);
    foreach(QString arg, requestArgs)
    {
        QString type = arg.replace("=","");
        if (type == "ping")
        {
            ping(client);
            break;
        }
        else if (type == "raise")
        {
            emit raise();
            break;
        }
    }
}

void Daemon::ping(QTcpSocket *client)
{
    QJsonObject obj;
    obj.insert("version", STR_VERSION);
    obj.insert("ramses", STR_INTERNALNAME);
    QJsonDocument json(obj);

    log("New request: ping", LogUtils::Information);
    client->write( json.toJson() );
}

Daemon::Daemon(QObject *parent) : QObject(parent)
{
    _tcpServer = new QTcpServer(this);

    start();

    connect(_tcpServer, &QTcpServer::newConnection, this, &Daemon::newConnection);
}
