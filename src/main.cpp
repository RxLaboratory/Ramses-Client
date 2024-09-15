#include <QApplication>
#include <QSettings>
#include <QTcpSocket>

#include "duapp/app-config.h"
#include "duapp/app-utils.h"
#include "duapp/ducli.h"
#include "duapp/dulogger.h"
#include "duwidgets/duqfupdatedialog.h"
#include "enums.h"
#include "ramsettings.h"
#include "src/qgoodwindow.h"

void initSettings(DuSplashScreen *s)
{
    s->newMessage("Reading settings...");

    RamSettings::registerRamsesSettings();

    QString prevVersionStr = DuSettings::i()->get(DuSettings::APP_Version).toString();
    QVersionNumber prevVersion = QVersionNumber::fromString( prevVersionStr );
    if (prevVersionStr != STR_VERSION)
    {
        // This is a new version,
        // Do stuff here if needed in this case
        // ...
        // And save
        DuSettings::i()->set(DuSettings::APP_Version, STR_VERSION);
    }
}

void initLogger(const QString &loglevel) {
    LogType level = InformationLog;
    if (loglevel == "debug") level = DebugLog;
    else if (loglevel == "warning") level = WarningLog;
    else if (loglevel == "error") level = CriticalLog;
    DuLogger::i()->setAsMessageHandler(level);
}

int main(int argc, char *argv[])
{
    QT_REQUIRE_VERSION(argc, argv, QT_VERSION_STR)

    QGoodWindow::setup();
    // Enable HiDPI / Retina
    DuUI::setUseHDPI();

    // The CLI
    QCommandLineParser parser;
    DuCLI::initParser(&parser);

    DuApplication a(argc, argv);
    a.processArgs();
    initLogger(parser.value("log_level"));

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

    // init settings
    initSettings(s);

    // Check for updates, right during startup
    s->newMessage("Looking for udpates...");
    QSettings settings;
    qDebug() << "Update check...";
    QDateTime lastCheck = DuSettings::i()->get(DuSettings::APP_LastUpdateCheck).toDateTime();
    qDebug().noquote() << "Last check was on: " + lastCheck.toString(DATETIME_DATA_FORMAT);
    int days = lastCheck.daysTo(QDateTime::currentDateTime());
    qDebug().noquote() << days << " days since last check.";
    if (days > 0 || !lastCheck.isValid() || lastCheck.isNull()) {
        a.checkUpdate();
        QJsonObject updateInfo = a.updateInfo();
        if (updateInfo.value("update").toBool()) {
            s->newMessage("A new version is available!");
            DuQFUpdateDialog dialog(updateInfo);
            if (DuUI::execDialog(&dialog)) return 0;
        }
        DuSettings::i()->set(DuSettings::APP_LastUpdateCheck, QDateTime::currentDateTime());
    }
    else
    {
        qDebug() << "We'll check again tomorrow.";
    }

    // build and show UI
    DuUI::buildUI(parser, a.splashScreen());

    return a.exec();
}
