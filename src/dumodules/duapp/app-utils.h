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
#include "duutils/duqflogger.h"

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

/**
 * @brief The DuApplication class
 * @version 1.1.0 Added restart() method
 */
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

    /**
     * @brief restart Exits the current app and restart the same executable.
     * This method will pass the original command line arguments to the new instance;
     * to restart without arguments, use restart(QStringList());
     */
    void restart();
    /**
     * @brief restart Exits the current app and restart the same executable
     * @param args The args to pass when starting the executable
     */
    void restart(QStringList args);

signals:
    void newUpdateInfo(QJsonObject);

private slots:
    void gotUpdateInfo(QNetworkReply *rep);

private:
    DuSplashScreen *_splashScreen;
    QJsonObject _updateInfo;

    // CLI
    QCommandLineParser *_cliParser = nullptr;
    QStringList _args;
    QString _program;

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
