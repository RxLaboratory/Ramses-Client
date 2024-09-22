#include <QApplication>
#include <QSettings>
#include <QTcpSocket>

#include "duapp/app-utils.h"
#include "duapp/ducli.h"
#include "duapp/dulogger.h"
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

    // build and show UI
    DuUI::buildUI(parser, a.splashScreen());

    return a.exec();
}
