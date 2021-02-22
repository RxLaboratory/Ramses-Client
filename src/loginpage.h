#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QScrollBar>

#include "ui_loginpage.h"

#include "daemon.h"
#include "ramses.h"

class LoginPage : public QWidget, private Ui::LoginPage
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

signals:
    void serverSettings();

private slots:
    void loggedIn(RamUser *user);
    void loggedOut();
    void dbiLog(QString m, LogUtils::LogType t);
    void daemonLog(QString m, LogUtils::LogType t);
    void dbiData(QJsonObject data);

    void loginButton_clicked();
    void serverSettingsButton_clicked();

    void showHideConsole();

private:
    Ramses *_ramses;
    QTimer *_failedTimer;
    QTimer *_uiTimer;
    int _failedAttempts;
    void freeze();
    void unFreeze();
    void updateFreeze();
};

#endif // LOGINPAGE_H
