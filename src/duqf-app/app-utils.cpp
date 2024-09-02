#include "app-utils.h"

#include <QStandardPaths>
#include <QDir>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "duqf-app/dusettings.h"
#include "duqf-app/duui.h"

DuApplication::DuApplication(int &argc, char *argv[], QCommandLineParser *parser) : QApplication(argc, argv)
{
    QString sharedMemoryKey = QString(STR_COMPANYDOMAIN) + "_" + QString(STR_PRODUCTNAME) + "_" + QString(STR_VERSION);
    m_singular = new QSharedMemory(sharedMemoryKey, this);

    // settings
    QCoreApplication::setOrganizationName(STR_COMPANYNAME);
    QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
    QCoreApplication::setApplicationName(STR_INTERNALNAME);
    QCoreApplication::setApplicationVersion(STR_VERSION);

    //create splash screen
    if (QFile(SPLASH_IMAGE).exists())
    {
        QPixmap pixmap(SPLASH_IMAGE);
        _splashScreen = new DuSplashScreen( pixmap );
    }
    else
    {
        _splashScreen = new DuSplashScreen( );
    }

    //set app icon
    qApp->setWindowIcon(QIcon(APP_ICON));

    //idle timer
    /*_idleTimer = new QTimer();
    connect(_idleTimer,SIGNAL(timeout()),this,SLOT(idleTimeOut()));
    _idleTimeout=120*60*1000;
    _idleTimer->start(_idleTimeout);*/

    // CLI
    if (parser) _cliParser = parser;
    else _cliParser = new QCommandLineParser();
    _cliParser->setApplicationDescription(STR_PRODUCTDESCRIPTION);
    _cliParser->addHelpOption();
    _cliParser->addVersionOption();

    // Connect some stuff
    connect(DuSettings::i(), &DuSettings::settingChanged,
            this, [](int k, const QVariant &v) {
                if (k == DuSettings::UI_ToolButtonStyle)
                    DuUI::setAppToolButtonStyle(
                        static_cast<Qt::ToolButtonStyle>( v.toInt() ));
            }
            );
}

DuApplication::~DuApplication()
{
    qDebug() << "Destroying app";
    if (m_singular->isAttached()) m_singular->detach();
}

bool DuApplication::lock()
{
    if (m_singular->attach(QSharedMemory::ReadOnly)){
        m_singular->detach();
        return false;
    }

    if (m_singular->create(1))
        return true;

    return false;
}

void DuApplication::detach()
{
    m_singular->detach();
}

DuSplashScreen *DuApplication::splashScreen() const
{
    return _splashScreen;
}

void DuApplication::showSplashScreen()
{
    _splashScreen->show();
}

/*void DuApplication::setIdleTimeOut(int to)
{
    _idleTimeout = to;
    _idleTimer->start(_idleTimeout);
}*/

void DuApplication::processArgs()
{
    // Process
    _cliParser->process(*this);

// Force no console without args on windows
#ifdef Q_OS_WIN
#ifndef QT_DEBUG
    if (this->arguments().count() < 2 || _cliParser->isSet("hide_console"))
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
#endif
#endif

    if (!_cliParser->isSet("hide_banner"))
    {
        qInfo().noquote() << STR_PRODUCTNAME;
        qInfo().noquote() << "version " + QString(STR_VERSION);
        qInfo().noquote() << STR_PRODUCTDESCRIPTION;
        qInfo().noquote() << STR_LEGALCOPYRIGHT;
        qInfo() << "";
        qInfo() << "This program comes with ABSOLUTELY NO WARRANTY;";
        qInfo() << "This is free software, and you are welcome to redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.";
        qInfo() << "";

#ifdef QT_DEBUG
        qInfo() << "Note: this is a development build and should not be used in production.";
        qInfo() << "";
#endif
    }

    if (_cliParser->isSet("reinit")) {
        QSettings().clear();
    }
}

void DuApplication::checkUpdate()
{
    if (QString(URL_UPDATE) == "") return;

// Get Sys info
#ifdef Q_OS_LINUX
    QString os("linux");
    QString distrib = QSysInfo::productType() % "-" % QSysInfo::productVersion();
    QString kernel = QSysInfo::kernelType() % "-" % QSysInfo::kernelVersion();
#endif
#ifdef Q_OS_WIN
    QString os("win");
    QString distrib = QSysInfo::productVersion();
    QString kernel = QSysInfo::kernelType() % "-" % QSysInfo::kernelVersion();
#endif
#ifdef Q_OS_MAC
    QString os("mac");
    QString distrib = QSysInfo::productType() % "-" % QSysInfo::productVersion();
    QString kernel = QSysInfo::kernelType() % "-" % QSysInfo::kernelVersion();
#endif
    // Build request
    QString serverAddress(URL_UPDATE);
    if (!serverAddress.endsWith("/")) serverAddress += "/";
    QUrl url(serverAddress %
             "?getVersion&name=" %
             QString(STR_INTERNALNAME) %
             "&version=" % QString(STR_VERSION) %
             "&os=" % os %
             "&osVersion=" % distrib % " (" % kernel % ")"
             );
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(STR_INTERNALNAME) + " v" + QString(STR_VERSION));
    QNetworkAccessManager *am = new QNetworkAccessManager(this);

    qInfo() << "Checking for update... This is the (only) info sent to the server at " % QString(URL_UPDATE) % ":";
    qInfo().noquote() << "OS: " % os;
    qInfo().noquote() << "OS Version: " % distrib % " (" % kernel % ")";
    qInfo().noquote() << "App Name: " % QString(STR_INTERNALNAME);
    qInfo().noquote() << "App Version: " % QString(STR_VERSION);

    connect(am, SIGNAL(finished(QNetworkReply*)), this, SLOT(gotUpdateInfo(QNetworkReply*)));
    am->get(request);
}

/*bool DuApplication::notify(QObject *receiver, QEvent *ev)
{
    if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::MouseButtonPress)
    {
        //reset timer
        _idleTimer->start(_idleTimeout);
    }
    return QApplication::notify(receiver,ev);
}*/

/*void DuApplication::idleTimeOut()
{
    emit idle();
}*/

void DuApplication::gotUpdateInfo(QNetworkReply *rep)
{
    if (rep->error() != QNetworkReply::NoError)
    {
        qDebug() << "Update Check failed!";
        qDebug() << rep->errorString();
        return;
    }

    QString repAll = rep->readAll();

    qDebug() << repAll;

    QJsonDocument repDoc = QJsonDocument::fromJson(repAll.toUtf8());
    _updateInfo = repDoc.object();

    if (_updateInfo.isEmpty())
    {
        _updateInfo.insert("message",repAll);
        _updateInfo.insert("accepted",false);
        _updateInfo.insert("success",false);
    }
    else if ( _updateInfo.value("success").toBool() )
    {
        QSettings settings;
        settings.setValue("updates/latestUpdateCheck", QDate::currentDate());
    }

    if (_updateInfo.value("success").toBool())
    {
        qInfo() << "Got update information:";
        if (_updateInfo.value("update").toBool())
        {
            qInfo().noquote() << "A new version is available! Version: " % _updateInfo.value("version").toString();
            qInfo().noquote() << "(Current version is: " % QString(STR_VERSION) % ")";
            qInfo().noquote() << "Update notes: " % _updateInfo.value("description").toString();
            qInfo().noquote() << "Detailed changelog: " % _updateInfo.value("changelogURL").toString();
            qInfo().noquote() << "Download: " % _updateInfo.value("downloadURL").toString();
            qInfo().noquote() << "Donate: " % _updateInfo.value("donateURL").toString();
        }
        else
        {
            qInfo() << "this version is already up-to-date, congrats!";
        }
    }

    emit newUpdateInfo(_updateInfo);

    rep->deleteLater();
}

const QJsonObject &DuApplication::updateInfo() const
{
    return _updateInfo;
}

QString AppUtils::issueReportUrl()
{
    QString url = QString(URL_BUGREPORT) +
                  QStringLiteral("?rx-tool=") +
                  QString(STR_INTERNALNAME) +
                  QStringLiteral("&rx-tool-version=") +
                  QString(STR_VERSION) +
                  QStringLiteral("&rx-host=DuME Application&rx-host-version=") +
                  QString(STR_VERSION) +
                  QStringLiteral("&rx-os=");

#ifdef Q_OS_LINUX
    url += "linux";
#endif
#ifdef Q_OS_WINDOWS
    url += "windows";
#endif
#ifdef Q_OS_MAC
    url += "mac";
#endif
    url += QStringLiteral("&rx-os-version=") + QSysInfo::productVersion();

    qDebug() << url;

    return url;
}

QString AppUtils::extractResource(QString resource, QString fileName, bool overwrite)
{
    QString p = QStandardPaths::locate(
        QStandardPaths::AppDataLocation,
        fileName
        );
    if (p != "" && !overwrite) return p;
    if (p != "") QFile::remove(p);

    QString dest = appDataPath();
    if (dest == "") return "";

    p = dest + "/" + fileName;
    QFile::copy( resource, p );

    if (QFileInfo::exists(p)) return p;

    return "";
}

QString AppUtils::appDataPath()
{
    QString p = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (!QFileInfo::exists(p)) QDir().mkpath(p);
    return p;
}
