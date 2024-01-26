#ifndef APPUTILS_H
#define APPUTILS_H

#include <QtDebug>
#include <QApplication>
#include <QIcon>
#include <QSplashScreen>
#include <QProgressBar>
#include <QLabel>
#include <QFileInfo>
#include <QFontDatabase>
#include <QRegularExpression>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStringBuilder>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSettings>

#include "app-version.h"
#include "../duqf-utils/duqflogger.h"

class MessageHandler
{
public:
    static void messageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
    {
        QByteArray localMsg = msg.toLocal8Bit();
        const char *file = context.file ? context.file : "";
        const char *function = context.function ? context.function : "";
        switch (type) {
        case QtDebugMsg:
    #ifdef QT_DEBUG
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
    #endif
            break;
        case QtInfoMsg:
            fprintf(stdout, "%s\n", localMsg.constData());
            break;
        case QtWarningMsg:
            fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtCriticalMsg:
            fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        case QtFatalMsg:
            fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), file, context.line, function);
            break;
        }
    }
};

class DuApplication : public QApplication
{
    Q_OBJECT
public:
    explicit DuApplication(int &argc, char *argv[]);
    ~DuApplication();

    bool lock();
    void detach();
    void restart(QStringList args = QStringList());

    DuSplashScreen *splashScreen();

    void showSplashScreen();

    void setIdleTimeOut(int to);

    // Process the CLI arguments
    bool processArgs(QStringList examples = QStringList(), QStringList helpStrings = QStringList());

    const QJsonObject &updateInfo();

    QStringList args() const;

public slots:
    // Check for updates
    void checkUpdate(bool wait = false);

signals:
    void idle();
    void newUpdateInfo(QJsonObject);

protected:
    virtual bool notify(QObject *receiver, QEvent *ev);

private slots:
    void idleTimeOut();
    void gotUpdateInfo(QNetworkReply *rep);

private:
    void createSplashScreen();

    DuSplashScreen *m_splashScreen = nullptr;
    QTimer *_idleTimer;
    int _idleTimeout;
    QString m_program;
    QStringList m_args;
    QJsonObject _updateInfo;

    // Used to check if another instance is running
    QSharedMemory *m_singular;
};

#endif // APPUTILS_H
