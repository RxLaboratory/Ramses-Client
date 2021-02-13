#include "mainwindow.h"

#include <QApplication>
#include <QSettings>

#ifdef Q_OS_WIN
#include "windows.h"
#endif

#include "duqf-app/app-utils.h"

int main(int argc, char *argv[])
{
    DuApplication a(argc, argv);

    // process CLI arguments
    if ( duqf_processArgs(argc, argv) ) return 0;

    // show splashscreen
    a.showSplashScreen();
    DuSplashScreen *s = a.splashScreen();

    // build and show UI
    s->newMessage("Building UI");
    MainWindow *w = new MainWindow( a.arguments() );
#ifndef Q_OS_LINUX
    FrameLessWindow f(w);
#endif
    w->show();

    // hide splash when finished
    s->finish(w);

    return a.exec();
}
