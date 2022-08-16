#include "loginpage.h"
#include "datacrypto.h"

#include "mainwindow.h"

LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

    ui_capsLockLabel->hide(); // TODO implement CAPS Lock detection

    m_ramses = Ramses::instance();
    m_failedTimer = new QTimer(this);
    m_failedTimer->setSingleShot(true);
    m_uiTimer = new QTimer(this);
    m_failedAttempts = 0;

    connectEvents();
}

void LoginPage::createDatabase()
{
    if (!ui_databaseCreateWidget)
    {
        ui_databaseCreateWidget = new DatabaseCreateWidget();
    }

    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->setPropertiesDockWidget( ui_databaseCreateWidget, tr("Create Ramses Database"), ":/icons/storage" );
}

void LoginPage::openDatabase()
{
    QString p = QFileDialog::getOpenFileName(this, tr("Open Ramses Database"), "", "Ramses DB (*.ramses);;SQLite (*.sqlite);;All Files (*.*)");
    if (p == "") return;

    // Add to combobox and select
    ui_dataBaseBox->insertItem(0, QFileInfo(p).fileName(), p);
    ui_dataBaseBox->setCurrentIndex(0);
}

void LoginPage::editDatabase()
{
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();

    if (!ui_databaseEditWidget)
    {
        ui_databaseEditWidget = new DatabaseEditWidget();
        connect(ui_databaseEditWidget, &DatabaseEditWidget::applied, mw, &MainWindow::hidePropertiesDock);
    }

    ui_databaseEditWidget->setDbFile( ui_dataBaseBox->itemData( ui_dataBaseBox->currentIndex() ).toString() );
    mw->setPropertiesDockWidget( ui_databaseEditWidget, tr("Edit") + " " + ui_dataBaseBox->currentText(), ":/icons/storage" );
}

void LoginPage::updateDatabaseRecentList()
{
    ui_dataBaseBox->clear();
    QSettings settings;
    int n = settings.beginReadArray("database/recent");
    for (int i = 0; i < n; i++)
    {
        settings.setArrayIndex(i);
        QString p = settings.value("path").toString();
        QFileInfo f(p);
        ui_dataBaseBox->addItem( f.fileName(), p);
    }
    settings.endArray();

    ui_dataBaseBox->setCurrentIndex(0);
}

void LoginPage::databaseChanged(int i)
{
    // Reset login fields
    ui_usernameEdit->setText("");
    ui_passwordEdit->setText("");
    ui_settingsDBButton->setEnabled(i >= 0);
    ui_loginButton->setEnabled(i >= 0);
    ui_passwordEdit->setEnabled(i >= 0);
    ui_usernameEdit->setEnabled(i >= 0);
    ui_saveUsername->setEnabled(i >= 0);
    ui_savePassword->setEnabled(i >= 0);
    if (i < 0) return;

    // Load saved credentials
    QSettings settings;
    QString dbPath = ui_dataBaseBox->itemData(i).toString();

    int historySize = settings.beginReadArray("database/recent");
    for (int i = 0; i < historySize; i++)
    {
        settings.setArrayIndex(i);
        // Get adress in settings
        QString settingsAddress = settings.value("path").toString();
        if (settingsAddress == dbPath )
        {
            // Decrypt
            DataCrypto *crypto = DataCrypto::instance();

            QString username = settings.value("username", "").toString();

            if (username == "")
            {
                ui_saveUsername->setChecked(false);
                ui_usernameEdit->setText("");
                ui_passwordEdit->setText("");
                ui_passwordEdit->setPlaceholderText("Password");
            }
            else
            {
                username = crypto->machineDecrypt( username );
                ui_usernameEdit->setText( username );
                ui_saveUsername->setChecked(true);

                QString password = settings.value("password", "").toString();
                if (password == "")
                {
                    ui_savePassword->setChecked(false);
                    ui_passwordEdit->setText("");
                    ui_passwordEdit->setPlaceholderText("Password");
                    m_hashedPassword = "";
                }
                else
                {
                    ui_savePassword->setChecked(true);
                    m_hashedPassword = crypto->machineDecrypt( password );
                    ui_passwordEdit->setPlaceholderText("Use saved password.");
                    ui_passwordEdit->setText("");
                }
            }

            break;
        }
    }
    settings.endArray();
}

void LoginPage::loggedIn(RamUser *user)
{
    // Save credentials
    if (user)
    {
        // Save credentials
        QSettings settings;
        QString dbPath = DBInterface::instance()->dataFile();

        qDebug() << "Saving credentials for " + dbPath;

        int historySize = settings.beginReadArray("database/recent");
        int historyIndex = 0;
        bool found = false;
        for (int i = 0; i < historySize; i++)
        {
            settings.setArrayIndex(i);
            // Get adress in settings
            QString settingsAddress = settings.value("path").toString();

            qDebug() << settingsAddress;

            if (settingsAddress == dbPath )
            {
                found = true;
                break;
            }
            historyIndex++;
        }
        settings.endArray();

        if (found)
        {
            qDebug() << "test";
            settings.beginWriteArray("database/recent");
            settings.setArrayIndex(historyIndex);

            // Save credentials
            if (ui_saveUsername->isChecked())
            {
                qDebug() << "test";
                // Encrypt
                DataCrypto *crypto = DataCrypto::instance();

                settings.setValue("username", crypto->machineEncrypt(ui_usernameEdit->text()));

                if (ui_savePassword->isChecked())
                {
                    // Save the hashed password
                    QString hashed;
                    if (ui_passwordEdit->text() == "" && ui_passwordEdit->placeholderText() == "Use saved password." && m_hashedPassword != "")
                    {
                        hashed = m_hashedPassword;
                    }
                    else
                    {
                        hashed = crypto->generatePassHash( ui_passwordEdit->text() );
                    }
                    // But encrypted, as the hashed password can be used to login
                    settings.setValue("password", crypto->machineEncrypt( hashed ));
                }
                else
                {
                    settings.setValue("password", "");
                }
            }
            else
            {
                settings.setValue("username", "");
                settings.setValue("password", "");
            }

            settings.endArray();
        }
    }

    ui_loginWidget->hide();
    updateDatabaseRecentList();
    ui_connectionStatusLabel->setText("Connected as " + user->name());

    ui_passwordEdit->setText("");
    if (ui_savePassword->isChecked())
        ui_passwordEdit->setPlaceholderText("Use saved password.");
    else
        ui_passwordEdit->setPlaceholderText("Password");

    if (!ui_saveUsername->isChecked())
        ui_saveUsername->setText("");
}

void LoginPage::loggedOut(QString reason)
{
    if (reason == "") reason = tr("Ready...");
    ui_loginWidget->show();
    ui_connectionStatusLabel->setText(reason);
}

void LoginPage::loginButton_clicked()
{
    //check login in database, initiate
    if (ui_usernameEdit->text() == "")
    {
        ui_connectionStatusLabel->setText("Please fill your username in.");
        return;
    }

    if (ui_passwordEdit->text() == "" && (ui_passwordEdit->placeholderText() != "Use saved password." || m_hashedPassword == ""))
    {
        ui_connectionStatusLabel->setText("Please fill your password in.");
        return;
    }

    // Set database
    DBInterface::instance()->setDataFile( ui_dataBaseBox->currentData().toString() );

    RamUser *user;

    if (ui_passwordEdit->text() == "")
    {
        user = m_ramses->loginHashed(ui_usernameEdit->text(), m_hashedPassword);
    }
    else
    {
        user = m_ramses->login(ui_usernameEdit->text(), ui_passwordEdit->text());
    }

    if (!user) freeze();
    else m_failedAttempts = 0;
}

void LoginPage::toggleSaveUsername(bool enabled)
{
    if (enabled) {
        ui_savePassword->setEnabled(true);
    }
    else {
        ui_savePassword->setEnabled(false);
        ui_savePassword->setChecked(false);
    }
}

void LoginPage::toggleSavePassword(bool enabled)
{
    if (!enabled) return;

    // Confirm that !

    QMessageBox::StandardButton result = QMessageBox::question(
                this,
                "Please confirm",
                "Saving the password is unsafe.\nAre you sure you want to save your password?",
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
                );
    if (result == QMessageBox::No) ui_savePassword->setChecked(false);
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

    QHBoxLayout *dataBaseLayout = new QHBoxLayout();
    dataBaseLayout->setSpacing(0);
    dataBaseLayout->setContentsMargins(0,0,0,0);
    loginLayout->addLayout(dataBaseLayout);

    ui_dataBaseBox = new QComboBox(this);
    dataBaseLayout->addWidget(ui_dataBaseBox);

    ui_settingsDBButton = new QPushButton(this);
    ui_settingsDBButton->setIcon(QIcon(":/icons/settings"));
    ui_settingsDBButton->setToolTip(tr("Database settings"));
    ui_settingsDBButton->setEnabled(false);
    dataBaseLayout->addWidget(ui_settingsDBButton);

    ui_openDBButton = new QPushButton(this);
    ui_openDBButton->setIcon(QIcon(":/icons/open"));
    ui_openDBButton->setToolTip(tr("Open database"));
    dataBaseLayout->addWidget(ui_openDBButton);

    ui_createDBButton = new QPushButton(this);
    ui_createDBButton->setIcon(QIcon(":/icons/add"));
    ui_createDBButton->setToolTip(tr("Create new database"));
    dataBaseLayout->addWidget(ui_createDBButton);

    dataBaseLayout->setStretch(0, 100);
    dataBaseLayout->setStretch(1, 0);

    QHBoxLayout *usernameLayout = new QHBoxLayout();
    usernameLayout->setSpacing(3);
    usernameLayout->setContentsMargins(0,0,0,0);
    loginLayout->addLayout(usernameLayout);

    ui_usernameEdit = new QLineEdit(this);
    ui_usernameEdit->setPlaceholderText(tr("Username"));
    usernameLayout->addWidget(ui_usernameEdit);

    ui_saveUsername = new QCheckBox("Save", this);
    usernameLayout->addWidget(ui_saveUsername);

    QHBoxLayout *passwordLayout = new QHBoxLayout();
    passwordLayout->setSpacing(3);
    passwordLayout->setContentsMargins(0,0,0,0);
    loginLayout->addLayout(passwordLayout);

    ui_passwordEdit = new QLineEdit(this);
    ui_passwordEdit->setEchoMode(QLineEdit::Password);
    ui_passwordEdit->setPlaceholderText(tr("Password"));
    passwordLayout->addWidget(ui_passwordEdit);

    ui_savePassword = new QCheckBox("Save", this);
    ui_savePassword->setEnabled(false);
    passwordLayout->addWidget(ui_savePassword);

    ui_capsLockLabel = new QLabel(tr("Be careful, CAPS LOCK is on!"), this);
    ui_capsLockLabel->setWordWrap(true);
    loginLayout->addWidget(ui_capsLockLabel);

    ui_loginButton = new QPushButton(tr("Log in"), this);
    ui_loginButton->setIcon(QIcon(":/icons/login"));
    loginLayout->addWidget(ui_loginButton);

    ui_connectionStatusLabel = new QLabel(tr("Ready..."), this);
    ui_connectionStatusLabel->setEnabled(false);
    ui_connectionStatusLabel->setWordWrap(true);
    ui_connectionStatusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(ui_connectionStatusLabel);

    ui_waitLabel = new QLabel(this);
    ui_waitLabel->setAlignment(Qt::AlignCenter);
    ui_waitLabel->hide();
    layout->addWidget(ui_waitLabel);

    layout->addStretch();
    mainLayout->addStretch();

    // Tab order
    QWidget::setTabOrder(ui_dataBaseBox, ui_usernameEdit);
    QWidget::setTabOrder(ui_usernameEdit, ui_passwordEdit);

    updateDatabaseRecentList();
    databaseChanged(ui_dataBaseBox->currentIndex());
}

void LoginPage::connectEvents()
{
    connect(ui_createDBButton, &QPushButton::clicked, this, &LoginPage::createDatabase);
    connect(ui_openDBButton, &QPushButton::clicked, this, &LoginPage::openDatabase);
    connect(ui_settingsDBButton, &QPushButton::clicked, this, &LoginPage::editDatabase);

    connect(ui_dataBaseBox, SIGNAL(currentIndexChanged(int)), this, SLOT(databaseChanged(int)));

    connect(ui_saveUsername, SIGNAL(toggled(bool)), this, SLOT(toggleSaveUsername(bool)));
    connect(ui_savePassword, SIGNAL(clicked(bool)), this, SLOT(toggleSavePassword(bool)));

    connect(m_ramses,&Ramses::loggedIn, this, &LoginPage::loggedIn);
    connect(m_ramses,&Ramses::loggedOut, this, &LoginPage::loggedOut);
    connect(ui_usernameEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    connect(ui_passwordEdit, &QLineEdit::returnPressed, this, &LoginPage::loginButton_clicked);
    //connect(DBInterface::instance(), &DBInterface::log, this, &LoginPage::dbiLog);
    //connect(Daemon::instance(), &Daemon::log, this, &LoginPage::daemonLog);
    connect(ui_loginButton, SIGNAL(clicked()), this, SLOT(loginButton_clicked()));
    connect(m_failedTimer, &QTimer::timeout, this, &LoginPage::unFreeze);
    connect(m_uiTimer, &QTimer::timeout, this, &LoginPage::updateFreeze);
}

void LoginPage::freeze()
{
    m_failedAttempts++;
    ui_loginWidget->hide();
    int timeout = m_failedAttempts * m_failedAttempts;
    qDebug() << "Freezing login page for " + QString::number(timeout) + " seconds";
    m_failedTimer->start(timeout*1000);
    m_uiTimer->start(1000);
    ui_waitLabel->show();
    updateFreeze();
}

void LoginPage::unFreeze()
{
    ui_loginWidget->show();
    m_uiTimer->stop();
    m_failedTimer->stop();
    ui_waitLabel->hide();
}

void LoginPage::updateFreeze()
{
    int remaining = m_failedTimer->remainingTime() / 1000;
    ui_waitLabel->setText(QString::number(m_failedAttempts) + " failed Attempts.\nPlease wait " + QString::number(remaining+1) + " seconds.");
}

