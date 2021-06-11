#include "app-utils.h"

DuApplication::DuApplication(int &argc, char *argv[]) : QApplication(argc, argv)
{
#ifndef QT_DEBUG
    // handles messages from the app and redirects them to stdout (info) or stderr (debug, warning, critical, fatal)
    qInstallMessageHandler(MessageHandler::messageOutput);
#endif
    qDebug() << "Initializing application";

    //set style
    DuUI::updateCSS(":/styles/default");

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

    // settings
    QCoreApplication::setOrganizationName(STR_COMPANYNAME);
    QCoreApplication::setOrganizationDomain(STR_COMPANYDOMAIN);
    QCoreApplication::setApplicationName(STR_PRODUCTNAME);
    QCoreApplication::setApplicationVersion(STR_VERSION);

    //idle timer
    _idleTimer = new QTimer();
    connect(_idleTimer,SIGNAL(timeout()),this,SLOT(idleTimeOut()));
    _idleTimeout=120*60*1000;
    _idleTimer->start(_idleTimeout);
}

DuSplashScreen *DuApplication::splashScreen() const
{
    return _splashScreen;
}

void DuApplication::showSplashScreen()
{
    _splashScreen->show();
}

void DuApplication::setIdleTimeOut(int to)
{
    _idleTimeout = to;
    _idleTimer->start(_idleTimeout);
}

bool DuApplication::processArgs(QStringList examples, QStringList helpStrings)
{
    bool nobanner = false;
    bool help = false;

    // No console without args on windows
#ifdef Q_OS_WIN
    bool hideConsole = _argc == 1;
#endif

    for (int i = 1; i < _argc; i++)
    {
        QString arg = _argv[i];
        if ( arg.toLower() == "--no-banner" ) nobanner = true;
        if (arg.toLower() == "-h" || arg.toLower() == "--help" ) help = true;
#ifdef Q_OS_WIN
        if (arg.toLower() == "--hide-console") hideConsole = true;
#endif
    }

    if (!nobanner)
    {
        qInfo().noquote() << STR_PRODUCTNAME;
        qInfo().noquote() << "version " + QString(STR_VERSION);
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

    if (help)
    {
        qInfo().noquote() << STR_PRODUCTDESCRIPTION;
        foreach(QString example, examples)
        {
            qInfo().noquote() << example;
        }
        qInfo() << "General";
        qInfo() << "    -h / --help       Print basic options without launching the application";
        qInfo().noquote() << "    See the documentation at " + QString(URL_DOC) + " for detailed descriptions of the options";
        qInfo() << "    --no-banner       Hides the banner with product information and legal notice";
        foreach(QString h, helpStrings)
        {
            qInfo().noquote() << h;
        }
#ifdef Q_OS_WIN
        qInfo() << "    --hide-console    Hides the console when starting the application using command line arguments";
#endif
    }

#ifdef Q_OS_WIN
#ifndef QT_DEBUG
    if (hideConsole)
    {
        ShowWindow(GetConsoleWindow(), SW_HIDE);
    }
#endif
#endif

    return help;
}

bool DuApplication::notify(QObject *receiver, QEvent *ev)
{
    if(ev->type() == QEvent::KeyPress || ev->type() == QEvent::MouseButtonPress)
    {
        //reset timer
        _idleTimer->start(_idleTimeout);
    }
    return QApplication::notify(receiver,ev);
}

void DuApplication::idleTimeOut()
{
    emit idle();
}
