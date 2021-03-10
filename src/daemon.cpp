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
        log("Unable to start the daemon server.\n" + _tcpServer->errorString(), DuQFLog::Warning);
    }
    else
    {
        qDebug() << "Daemon started and listening on port " + QString::number(_tcpServer->serverPort());
        log("Daemon started and listening on port " + QString::number(_tcpServer->serverPort()), DuQFLog::Information);
    }
}

void Daemon::stop()
{
    _tcpServer->close();
    log("Daemon stopped.", DuQFLog::Information);
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
    log("Got args: \n" + requestArgs.join("\n"), DuQFLog::Debug);
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

    log("Daemon received a new request: ping", DuQFLog::Information);
    client->write( json.toJson() );
}

Daemon::Daemon(QObject *parent) : DuQFLoggerObject("Daemon", parent)
{
    _tcpServer = new QTcpServer(this);

    start();

    connect(_tcpServer, &QTcpServer::newConnection, this, &Daemon::newConnection);
}
