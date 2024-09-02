#ifndef DUAPPLICATION_H
#define DUAPPLICATION_H

#include <QApplication>
#include <QCommandLineParser>

#include "duqf-widgets/dusplashscreen.h"
#include "qjsonobject.h"
#include "qnetworkreply.h"
#include "qsharedmemory.h"

class DuApplication : public QApplication
{
    Q_OBJECT
public:
    explicit DuApplication(int &argc, char *argv[], QCommandLineParser *parser = nullptr);
    ~DuApplication();

    bool lock();
    void detach();
    void restart(QStringList args = QStringList());

    DuSplashScreen *splashScreen() const;
    void showSplashScreen();
    void releaseSplashScreen();

    const QJsonObject &updateInfo();

signals:
    void newUpdateInfo(QJsonObject);

public slots:
    // Check for updates
    void checkUpdate(bool wait = false);

protected:
    void processArgs();
    void initLogger(const QString &loglevel = "information");
    void initSettings();

private slots:
    void gotUpdateInfo(QNetworkReply *rep);

private:
    DuSplashScreen *_splashScreen;
    QCommandLineParser *_cliParser = nullptr;
    QJsonObject _updateInfo;
    QStringList _args;
    QString _program;
    // Used to check if another instance is running
    QSharedMemory *_singular;
};

#endif // DUAPPLICATION_H
