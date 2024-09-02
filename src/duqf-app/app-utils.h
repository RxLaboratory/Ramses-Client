#ifndef APPUTILS_H
#define APPUTILS_H

#include <QtDebug>
#include <QApplication>
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
#include <QSharedMemory>

#include "app-version.h"
#include "duui.h"
#include "duqf-utils/duqflogger.h"

class DuSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    DuSplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags()) : QSplashScreen(pixmap, f)
    {
        //CSS
        this->setStyleSheet( DuUI::css("QSplashScreen") );
        //add a progress bar
        _progressBar = new QProgressBar(this);
        _progressBar->setMinimumWidth( pixmap.width() );
        _progressBar->setMaximumWidth( pixmap.width() );
        _progressBar->move( 0, pixmap.height() - 25);
        _progressBar->setAlignment(Qt::AlignVCenter);
        //add a label for the version
        _versionLabel = new QLabel("v" + QString(STR_VERSION), this);
        _versionLabel->setMinimumWidth( pixmap.width() );
        _versionLabel->setMaximumWidth( pixmap.width() );
        _versionLabel->move( 0, pixmap.height() - 40);
    }

public slots:
    void newMessage(QString message, DuQFLog::LogType lt = DuQFLog::Information)
    {
        if (!this->isVisible()) return;
        if (lt == DuQFLog::Debug) return;
        _progressBar->setFormat( "%p% - " + message );
#ifdef QT_DEBUG
        qDebug().noquote() << message;
#endif
        repaint();
    }
    void progressMax(int max)
    {
        if (!this->isVisible()) return;
        _progressBar->setMaximum( max );
    }
    void progress(int val)
    {
        if (!this->isVisible()) return;
        _progressBar->setValue(val);
        repaint();
    }

private:

    QProgressBar *_progressBar;
    QLabel *_versionLabel;
};

class DuApplication : public QApplication
{
    Q_OBJECT
public:
    explicit DuApplication(int &argc, char *argv[], QCommandLineParser *parser = nullptr);
    ~DuApplication();

    bool lock();
    void detach();

    DuSplashScreen *splashScreen() const;

    void showSplashScreen();

    void setIdleTimeOut(int to);

    // Process the CLI arguments
    void processArgs();

    const QJsonObject &updateInfo() const;

public slots:
    // Check for updates
    void checkUpdate();

signals:
    //void idle();
    void newUpdateInfo(QJsonObject);

protected:
           //virtual bool notify(QObject *receiver, QEvent *ev);

private slots:
    //void idleTimeOut();
    void gotUpdateInfo(QNetworkReply *rep);

private:
    DuSplashScreen *_splashScreen;
    //QTimer *_idleTimer;
    //int _idleTimeout;
    QJsonObject _updateInfo;
    QCommandLineParser *_cliParser = nullptr;

    // Used to check if another instance is running
    QSharedMemory *m_singular;
};

namespace AppUtils
{
QString issueReportUrl();
/**
     * @brief Gets, and creates if needed, the app data path
     * @return
     */
QString appDataPath();
/**
     * @brief Saves a file from the resources
     * @param resource The resource
     * @param dest The destination. If empty, AppData/RxLaboratory/AppName
     * @return
     */
QString extractResource(QString resource, QString fileName, bool overwrite = true);
}

#endif // APPUTILS_H
