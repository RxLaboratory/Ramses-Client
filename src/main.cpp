#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QTcpSocket>

#include "duqf-app/app-utils.h"
#include "duqf-widgets/duqfupdatedialog.h"

int main(int argc, char *argv[])
{
    DuApplication a(argc, argv);

    // process CLI arguments
    if ( a.processArgs() ) return 0;

    // Check if we're already running
    if (!a.lock()) {
        qInfo() << "Ramses is already running, sorry.";

        qDebug() << "Trying to connect to the running instance, sending args and waking it up.";

        QTcpSocket *tcpSocket = new QTcpSocket();
        tcpSocket->connectToHost(
                    "127.0.01",
                    QSettings().value("server/port", 18185).toInt(),
                    QIODevice::ReadWrite,
                    QAbstractSocket::IPv4Protocol
                    );
        tcpSocket->waitForConnected(1000);

        if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
            tcpSocket->write("raise");
            tcpSocket->waitForBytesWritten(100);
        }
        return -1;
    }

    // show splashscreen
    a.showSplashScreen();
    DuSplashScreen *s = a.splashScreen();
    s->newMessage("Checking if Ramses is already running...");

    // Check for updates, right during startup
    s->newMessage("Looking for udpates...");
    QSettings settings;
    qDebug() << "Update check...";
    QDateTime lastCheck = settings.value("updates/latestUpdateCheck").toDateTime();
    qDebug().noquote() << "Last check was on: " + lastCheck.toString("yyyy-MM-dd hh:mm:ss");
    int days = lastCheck.daysTo(QDateTime::currentDateTime());
    qDebug().noquote() << days << " days since last check.";
    if (days > 0 || !lastCheck.isValid() || lastCheck.isNull()) {
        a.checkUpdate(true);
        QJsonObject updateInfo = a.updateInfo();
        if (updateInfo.value("update").toBool()) {
            s->newMessage("A new version is available!");
            DuQFUpdateDialog dialog(updateInfo);
            if (dialog.exec()) return 0;
        }
        settings.setValue("updates/latestUpdateCheck", QDateTime::currentDateTime());
    }
    else
    {
        qDebug() << "We'll check again tomorrow.";
    }

    // build and show UI
    s->newMessage("Building UI");
    MainWindow *w = new MainWindow( a.arguments() );
#ifndef Q_OS_LINUX
    //FrameLessWindow f(w);
#endif
    w->show();

    // hide splash when finished
    s->newMessage("Ready!");
    s->finish(w);//*/

    return a.exec();
}
