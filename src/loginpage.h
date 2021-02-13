#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include "ui_loginpage.h"

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

    void loginButton_clicked();
    void serverSettingsButton_clicked();

private:
    Ramses *_ramses;

};

#endif // LOGINPAGE_H
