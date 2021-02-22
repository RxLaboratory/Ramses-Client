#include "loginpage.h"

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _ramses = Ramses::instance();
    _failedTimer = new QTimer(this);
    _failedTimer->setSingleShot(true);
    _uiTimer = new QTimer(this);
    _failedAttempts = 0;

#ifdef QT_DEBUG
    // Test mode (auto login)
    usernameEdit->setText("Duduf");
    passwordEdit->setText("password");
#endif

    QList<int> sizes;
    sizes << 0;
    sizes << 100;
    splitter->setSizes(sizes);

    connect(_ramses,&Ramses::loggedIn, this, &LoginPage::loggedIn);
    connect(_ramses,&Ramses::loggedOut, this, &LoginPage::loggedOut);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    connect(DBInterface::instance(), &DBInterface::log, this, &LoginPage::dbiLog);
    connect(Daemon::instance(), &Daemon::log, this, &LoginPage::daemonLog);
    connect(DBInterface::instance(), &DBInterface::data, this, &LoginPage::dbiData);
    connect(serverSettingsButton, SIGNAL(clicked()), this, SLOT(serverSettingsButton_clicked()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(loginButton_clicked()));
    connect(_failedTimer, &QTimer::timeout, this, &LoginPage::unFreeze);
    connect(_uiTimer, &QTimer::timeout, this, &LoginPage::updateFreeze);
    connect(consoleButton, &QToolButton::clicked, this, &LoginPage::showHideConsole);
}

void LoginPage::loggedIn(RamUser *user)
{
    usernameEdit->setText(user->shortName());
    passwordEdit->setText("");
    loginWidget->hide();
    connectionStatusLabel->setText("Connected as " + user->name());
}

void LoginPage::loggedOut()
{
    loginWidget->show();
    connectionStatusLabel->setText("Ready");
}

void LoginPage::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) connectionStatusLabel->setText(m);

    //type
    QString typeString = "";
    if (t == LogUtils::Warning)
    {
        typeString = "/!\\ Warning: ";
    }
    else if (t == LogUtils::Critical)
    {
        typeString = " --- !!! Critical: ";
    }
    else if (t == LogUtils::Fatal)
    {
        typeString = " === Fatal === ";
    }
    else if (t == LogUtils::Remote)
    {
        typeString = ":: Remote ::\n";
    }

#ifndef QT_DEBUG
    if ( t != LogUtils::Debug && t != LogUtils::Debug)
    {
#endif
        //add date
        QTime currentTime = QTime::currentTime();
        console->setTextColor(QColor(109,109,109));
        console->setFontWeight(300);
        console->append(currentTime.toString("[hh:mm:ss.zzz]: "));
        console->moveCursor(QTextCursor::End);
        if (t == LogUtils::Information) console->setTextColor(QColor(227,227,227));
        else if (t == LogUtils::Warning) console->setTextColor(QColor(236,215,24));
        else if (t == LogUtils::Critical) console->setTextColor(QColor(249,105,105));
        console->setFontWeight(800);
        console->setFontItalic(true);
        console->insertPlainText(typeString);
        console->setFontWeight(400);
        console->setFontItalic(false);
        console->insertPlainText(m);
        console->verticalScrollBar()->setSliderPosition(console->verticalScrollBar()->maximum());
#ifndef QT_DEBUG
    }
#endif
}

void LoginPage::daemonLog(QString m, LogUtils::LogType t)
{
    if (t == LogUtils::Debug || t == LogUtils::Remote)
    {
        m = "Daemon says: " + m;
    }
    dbiLog(m, t);
}

void LoginPage::dbiData(QJsonObject data)
{
    if (data.value("query").toString() == "login")
    {
        if (data.value("success").toBool())
        {
            unFreeze();
            _failedAttempts = 0;
        }
        else
        {
            _failedAttempts++;
            freeze();
        }
    }
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

void LoginPage::showHideConsole()
{
    QList<int> sizes = splitter->sizes();
    if (sizes[0] < 100)
    {
        sizes[0] = 500;
        sizes[1] = 500;
        splitter->setSizes(sizes);
    }
    else
    {
        sizes[0] = 0;
        sizes[1] = 100;
        splitter->setSizes(sizes);
    }
}

void LoginPage::freeze()
{
    loginWidget->hide();
    int timeout = _failedAttempts * _failedAttempts;
    qDebug() << "Freezing login page for " + QString::number(timeout) + " seconds";
    _failedTimer->start(timeout*1000);
    _uiTimer->start(1000);
    connectionStatusLabel->setEnabled(true);
}

void LoginPage::unFreeze()
{
    loginWidget->show();
    connectionStatusLabel->setText("Ready.");
    connectionStatusLabel->setEnabled(false);
    _uiTimer->stop();
    _failedTimer->stop();
}

void LoginPage::updateFreeze()
{
    int remaining = _failedTimer->remainingTime() / 1000;
    connectionStatusLabel->setText(QString::number(_failedAttempts) + " failed Attempts.\nPlease wait " + QString::number(remaining) + " seconds.");
}

