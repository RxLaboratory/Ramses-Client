#include "mainwindow.h"

#include <QApplication>
#include <QSettings>
#include <QTcpSocket>

#ifdef Q_OS_WIN
#include "FramelessWindow/QWinWidget.h"
#else
#include "UI/mainwindow.h"
#endif

#ifdef __APPLE__
#include "FramelessWindow/OSXHideTitleBar.h"
#endif

#include "duqf-app/app-utils.h"
#include "duqf-widgets/duqfupdatedialog.h"
#include "duqf-app/dusettingsmanager.h"

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

        bool mustQuit = true;

        if (tcpSocket->state() == QAbstractSocket::ConnectedState) {
            // Open the file if one is given
            if (argc == 2) {
                QString filePath = argv[1];
                QFileInfo argInfo(filePath);
                if (argInfo.exists() && argInfo.suffix().toLower() == "ramses") {
                    qDebug() << "Opening file " << argInfo.fileName();
                    // Quit other instance
                    tcpSocket->write(QString("quit").toUtf8());
                    mustQuit = false;
                }
            }

            if (mustQuit) {
                tcpSocket->write("raise");
                tcpSocket->waitForBytesWritten(100);
            }
        }

        if (mustQuit) return -1;
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

#ifdef _WIN32

    //On Windows, the widget needs to be encapsulated in a native window for frameless rendering
    //In this case, QWinWidget creates the mainwindow, and adds it to a layout
    QWinWidget *w = new QWinWidget(a.arguments());

    // Restore Geometry and state
    w->restoreGeometry( DuSettingsManager::instance()->uiWindowGeometry() );
    w->getMainWindow()->restoreState(DuSettingsManager::instance()->uiWindowState());

#else
    //On OS X / Linux, the widget can handle everything itself so just create Widget as normal
    MainWindow *w = new MainWindow( args );

    // Restore Geometry and state
    w->restoreGeometry( DuSettingsManager::instance()->uiWindowGeometry() );
    w->restoreState(DuSettingsManager::instance()->uiWindowState());
    // But hide the docks for a cleaner look
    w->hideAllDocks();
#endif

    w->show();

    //hide splash when finished
    s->finish(w);

    return a.exec();
}
