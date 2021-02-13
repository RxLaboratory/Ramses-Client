#include "loginpage.h"

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ramses = Ramses::instance();

#ifdef QT_DEBUG
    // Test mode (auto login)
    usernameEdit->setText("Admin");
    passwordEdit->setText("password");
#endif

    connect(_ramses,&Ramses::loggedIn, this, &LoginPage::loggedIn);
    connect(_ramses,&Ramses::loggedOut, this, &LoginPage::loggedOut);
    connect(DBInterface::instance(), &DBInterface::log, this, &LoginPage::dbiLog);
    connect(serverSettingsButton, SIGNAL(clicked()), this, SLOT(serverSettingsButton_clicked()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(loginButton_clicked()));
}

void LoginPage::loggedIn(RamUser *user)
{
    usernameEdit->setEnabled(false);
    usernameEdit->setText(user->shortName());
    passwordEdit->setEnabled(false);
    passwordEdit->setText("");
    loginButton->setEnabled(false);
    connectionStatusLabel->setText("Connected as " + user->name());
}

void LoginPage::loggedOut()
{
    usernameEdit->setEnabled(true);
    passwordEdit->setEnabled(true);
    loginButton->setEnabled(true);
    connectionStatusLabel->setText("Ready");
}

void LoginPage::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) connectionStatusLabel->setText(m);
}

void LoginPage::loginButton_clicked()
{
    //check login in database, initiate
    if (usernameEdit->text() == "")
    {
        connectionStatusLabel->setText("Please fill your username in.");
        return;
    }
    if (passwordEdit->text() == "")
    {
        connectionStatusLabel->setText("Please fill your password in.");
        return;
    }

    _ramses->login(usernameEdit->text(), passwordEdit->text());
    connectionStatusLabel->setText("Connecting...");
}

void LoginPage::serverSettingsButton_clicked()
{
    emit serverSettings();
}

