#include <QApplication>
#include <QSettings>
#include <QTcpSocket>

#include "duqf-app/duapplication.h"
#include "duqf-app/ducli.h"
#include "duqf-widgets/duqfupdatedialog.h"

int main(int argc, char *argv[])
{
    QT_REQUIRE_VERSION(argc, argv, QT_VERSION_STR)

    // Enable HiDPI / Retina

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0) && defined(Q_OS_WIN)
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    // The CLI
    QCommandLineParser parser;
    DuCLI::initParser(&parser);

    DuApplication a(argc, argv);

    // show splashscreen
    a.showSplashScreen();
    DuSplashScreen *s = a.splashScreen();
    s->newMessage("Checking if Ramses is already running...");

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
    DuUI::buildUI(parser, a.splashScreen());

    return a.exec();
}
