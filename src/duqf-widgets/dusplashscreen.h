#ifndef DUSPLASHSCREEN_H
#define DUSPLASHSCREEN_H

#include <QSplashScreen>
#include <QProgressBar>
#include <QtDebug>
#include <QLabel>

#include "duqf-app/duui.h"
#include "enums.h"
#include "duqf-app/app-version.h"

class DuSplashScreen : public QSplashScreen
{
    Q_OBJECT
public:
    DuSplashScreen(const QPixmap &pixmap = QPixmap(), Qt::WindowFlags f = Qt::WindowFlags()):
        QSplashScreen(pixmap, f)
    {
        //CSS
        this->setProperty("class", "duSplash");
        this->setStyleSheet( DuUI::css("duSplash") );
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
    void newMessage(QString message, LogType lt = InformationLog)
    {
        if (!this->isVisible()) return;
        if (lt == DebugLog) return;
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

#endif // DUSPLASHSCREEN_H
