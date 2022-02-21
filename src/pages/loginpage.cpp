#include "loginpage.h"

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    ui_capsLockLabel->hide(); // TODO implement CAPS Lock detection

    _ramses = Ramses::instance();
    _failedTimer = new QTimer(this);
    _failedTimer->setSingleShot(true);
    _uiTimer = new QTimer(this);
    _failedAttempts = 0;

#ifdef QT_DEBUG
    // Test mode (auto login)
    ui_usernameEdit->setText("Duf");
    ui_passwordEdit->setText("password");
#endif

    connectEvents();
}

void LoginPage::loggedIn(RamUser *user)
{
    ui_usernameEdit->setText(user->shortName());
    ui_passwordEdit->setText("");
    ui_loginWidget->hide();
    ui_connectionStatusLabel->setText("Connected as " + user->name());

    // Save server address to history
    QSettings settings;
    QString address = settings.value("server/address", "localhost/ramses/").toString();
    bool found = false;
    int historySize = settings.beginReadArray("server/serverHistory");
    for (int i = 0; i < historySize; i++)
    {
        settings.setArrayIndex(i);
        if (settings.value("address").toString() == address )
        {
            found = true;
            break;
        }
    }
    settings.endArray();
    if (!found)
    {
        settings.beginWriteArray("server/serverHistory");
        settings.setArrayIndex(historySize);
        settings.setValue("address", address);
        settings.endArray();
    }
}

void LoginPage::loggedOut()
{
    ui_loginWidget->show();
    ui_connectionStatusLabel->setText("Ready");
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
    if (ui_usernameEdit->text() == "")
    {
        ui_connectionStatusLabel->setText("Please fill your username in.");
        return;
    }
    if (ui_passwordEdit->text() == "")
    {
        ui_connectionStatusLabel->setText("Please fill your password in.");
        return;
    }

    _ramses->login(ui_usernameEdit->text(), ui_passwordEdit->text());
    ui_connectionStatusLabel->setText("Connecting...");
}

void LoginPage::serverSettingsButton_clicked()
{
    emit serverSettings();
}

void LoginPage::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setSpacing(3);
    mainLayout->addLayout(layout);

    layout->addStretch();

    QLabel *logoLabel = new QLabel(this);
    logoLabel->setPixmap(QPixmap(":/icons/logo_large"));
    logoLabel->setMinimumSize(QSize(256, 298));
    logoLabel->setMaximumSize(QSize(256, 298));
    logoLabel->setScaledContents(true);
    logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(logoLabel);

    layout->addStretch();

    ui_loginWidget = new QWidget(this);
    ui_loginWidget->setMaximumWidth(256);
    layout->addWidget(ui_loginWidget);

    QVBoxLayout *loginLayout = new QVBoxLayout(ui_loginWidget);
    loginLayout->setSpacing(3);
    loginLayout->setContentsMargins(0,0,0,0);

    QHBoxLayout *serverLayout = new QHBoxLayout();
    serverLayout->setSpacing(3);
    serverLayout->setContentsMargins(0,0,0,0);
    loginLayout->addLayout(serverLayout);

    ui_serverBox = new DuQFServerComboBox("localhost/ramses", this);
    serverLayout->addWidget(ui_serverBox);

    ui_sslBox = new DuQFSSLCheckbox(this);
    serverLayout->addWidget(ui_sslBox);

    serverLayout->setStretch(0, 100);
    serverLayout->setStretch(1, 0);

    ui_usernameEdit = new QLineEdit(this);
    ui_usernameEdit->setPlaceholderText("Username");
    ui_usernameEdit->setAlignment(Qt::AlignCenter);
    loginLayout->addWidget(ui_usernameEdit);

    ui_passwordEdit = new QLineEdit(this);
    ui_passwordEdit->setEchoMode(QLineEdit::Password);
    ui_passwordEdit->setPlaceholderText("Password");
    ui_passwordEdit->setAlignment(Qt::AlignCenter);
    loginLayout->addWidget(ui_passwordEdit);

    ui_capsLockLabel = new QLabel("Be careful, CAPS LOCK is on!", this);
    ui_capsLockLabel->setWordWrap(true);
    loginLayout->addWidget(ui_capsLockLabel);

    ui_loginButton = new QPushButton("Log in", this);
    ui_loginButton->setIcon(QIcon(":/icons/login"));
    loginLayout->addWidget(ui_loginButton);

    ui_connectionStatusLabel = new QLabel("Ready...", this);
    ui_connectionStatusLabel->setEnabled(false);
    ui_connectionStatusLabel->setWordWrap(true);
    ui_connectionStatusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(ui_connectionStatusLabel);

    layout->addStretch();
    mainLayout->addStretch();
}

void LoginPage::connectEvents()
{
    connect(_ramses,&Ramses::loggedIn, this, &LoginPage::loggedIn);
    connect(_ramses,&Ramses::loggedOut, this, &LoginPage::loggedOut);
    connect(ui_usernameEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    connect(ui_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    //connect(DBInterface::instance(), &DBInterface::log, this, &LoginPage::dbiLog);
    //connect(Daemon::instance(), &Daemon::log, this, &LoginPage::daemonLog);
    connect(DBInterface::instance(), &DBInterface::data, this, &LoginPage::dbiData);
    connect(ui_loginButton, SIGNAL(clicked()), this, SLOT(loginButton_clicked()));
    connect(_failedTimer, &QTimer::timeout, this, &LoginPage::unFreeze);
    connect(_uiTimer, &QTimer::timeout, this, &LoginPage::updateFreeze);
}

void LoginPage::freeze()
{
    ui_loginWidget->hide();
    int timeout = _failedAttempts * _failedAttempts;
    qDebug() << "Freezing login page for " + QString::number(timeout) + " seconds";
    _failedTimer->start(timeout*1000);
    _uiTimer->start(1000);
    ui_connectionStatusLabel->setEnabled(true);
}

void LoginPage::unFreeze()
{
    ui_loginWidget->show();
    ui_connectionStatusLabel->setText("Ready.");
    ui_connectionStatusLabel->setEnabled(false);
    _uiTimer->stop();
    _failedTimer->stop();
}

void LoginPage::updateFreeze()
{
    int remaining = _failedTimer->remainingTime() / 1000;
    ui_connectionStatusLabel->setText(QString::number(_failedAttempts) + " failed Attempts.\nPlease wait " + QString::number(remaining) + " seconds.");
}

