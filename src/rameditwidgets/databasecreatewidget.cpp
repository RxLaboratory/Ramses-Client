#include "databasecreatewidget.h"

#include "dbinterface.h"
#include "duqf-utils/utils.h"
#include "ramuser.h"
#include "ramses.h"
#include "mainwindow.h"

DatabaseCreateWidget::DatabaseCreateWidget(QWidget *parent) :
    QScrollArea(parent)
{
    setupUi();
    connectEvents();
}

void DatabaseCreateWidget::checkPath(QString p)
{
    if (!p.endsWith(".ramses", Qt::CaseInsensitive) && !p.endsWith(".sqlite", Qt::CaseInsensitive))
    {
        p += ".ramses";
        ui_fileSelector->setPath(p);
    }
}

void DatabaseCreateWidget::createDB()
{
    // Offline
    if (ui_tabWidget->currentIndex() == 0)
    {
        // Check path
        if (ui_fileSelector->path() == "")
        {
            QMessageBox::warning(this, tr("Where should I save the database?"), tr("I'm sorry, you have to choose a file to create the database.") );
            return;
        }

        // Check password
        if (ui_npassword1Edit->text() == "")
        {
            QMessageBox::warning(this, tr("What's your password?"), tr("I'm sorry, you have to choose a password to create the database.") );
            return;
        }
        if (ui_npassword1Edit->text() != ui_npassword2Edit->text())
        {
            QMessageBox::warning(this, tr("Password mismatch"), tr("I'm sorry, the two fields for the new password are different.\nPlease try again.") );
            ui_npassword1Edit->setText("");
            ui_npassword2Edit->setText("");
            return;
        }

        // Check name
        if (ui_shortNameEdit->text() == "")
        {
            QMessageBox::warning(this, tr("What's your name?"), tr("I'm sorry, you have to choose a user ID to create the database.") );
            return;
        }

        // Remove existing file
        QString newFilePath = ui_fileSelector->path();
        if (QFileInfo::exists(newFilePath))
        {
            QMessageBox::StandardButton ok = QMessageBox::question(this, tr("Confirm file overwrite"), tr("Are you sure you want to overwrite this file?") + "\n\n" + newFilePath);
            if (ok != QMessageBox::Yes) return;
            FileUtils::remove(newFilePath);
        }

        // Copy the file
        FileUtils::copy(":/data/template", newFilePath);

        if (!QFileInfo::exists(newFilePath))
        {
            QMessageBox::warning(this, tr("I can't save the database"), tr("I'm sorry, I've failed to create the database at this location.\nMaybe you can try another location...") + "\n\n" + newFilePath );
            return;
        }

        // Set File
        DBInterface::instance()->setDataFile(newFilePath);

        // Create user
        RamUser *newUser = new RamUser(ui_shortNameEdit->text(), ui_shortNameEdit->text());
        newUser->updatePassword("", ui_npassword1Edit->text());
        newUser->setRole(RamUser::Admin);

        // Login
        RamUser *currentUser = Ramses::instance()->login(ui_shortNameEdit->text(), ui_npassword1Edit->text());
        if (!currentUser)
        {
            QMessageBox::warning(this, tr("I can't log in"), tr("I'm sorry, I can't log in after the database creation.\nThat's probably a bug...") );
            return;
        }

        // Hide dock
        MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
        mw->hidePropertiesDock();
    }
    // Online
    else
    {
        // Connect to  server

        // Download all data

        // Create DB

        // Save data to DB
    }
}

void DatabaseCreateWidget::setupUi()
{
    QWidget *dummy = new QWidget(this);
    dummy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setWidget(dummy);
    this->setWidgetResizable(true);
    this->setFrameStyle(QFrame::NoFrame);

    QVBoxLayout *mainLayout = new QVBoxLayout(dummy);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3, 3, 3, 3);

    ui_fileSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::File, dummy);
    ui_fileSelector->setPlaceHolderText(tr("File path of the Ramses Database..."));
    ui_fileSelector->setDialogTitle(tr("Select the location of the Ramses Database."));
    ui_fileSelector->setMode(DuQFFolderSelectorWidget::Save);
    ui_fileSelector->setFilter(tr("Ramses (*.ramses);;SQLite (*.sqlite);;All Files (*.*)"));
    mainLayout->addWidget(ui_fileSelector);

    ui_tabWidget = new QTabWidget(dummy);
    ui_tabWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    mainLayout->addWidget(ui_tabWidget);

    QWidget *offlineWidget = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(offlineWidget, QIcon(":/icons/folder"), tr("Offline"));
    QGridLayout *offlineLayout = new QGridLayout(offlineWidget);
    offlineLayout->setAlignment(Qt::AlignTop);

    QLabel *userLabel = new QLabel("User ID", dummy);
    offlineLayout->addWidget(userLabel, 0, 0);

    ui_shortNameEdit = new QLineEdit(dummy);
    offlineLayout->addWidget(ui_shortNameEdit, 0, 1);

    QLabel *newPasswordLabel = new QLabel(tr("New password"), this);
    offlineLayout->addWidget(newPasswordLabel, 1, 0);

    ui_npassword1Edit = new QLineEdit(this);
    ui_npassword1Edit->setEchoMode(QLineEdit::Password);
    offlineLayout->addWidget(ui_npassword1Edit, 1, 1);

    QLabel *newPasswordLabel2 = new QLabel(tr("Repeat new password"), this);
    offlineLayout->addWidget(newPasswordLabel2, 2, 0);

    ui_npassword2Edit = new QLineEdit(this);
    ui_npassword2Edit->setEchoMode(QLineEdit::Password);
    offlineLayout->addWidget(ui_npassword2Edit, 2, 1);

    QWidget *onlineWidget = new QWidget(ui_tabWidget);
    ui_tabWidget->addTab(onlineWidget, QIcon(":/icons/server-settings"), tr("Online (Sync)"));
    QGridLayout *onlineLayout = new QGridLayout(onlineWidget);
    onlineLayout->setAlignment(Qt::AlignTop);

    QLabel *adressLabel = new QLabel("Server Address", onlineWidget);
    onlineLayout->addWidget(adressLabel, 0, 0);

    QHBoxLayout *adressLayout = new QHBoxLayout();
    adressLayout->setSpacing(3);
    adressLayout->setContentsMargins(0,0,0,0);
    onlineLayout->addLayout(adressLayout, 0, 1);

    QLabel *protocolLabel = new QLabel("http(s)://", onlineWidget);
    adressLayout->addWidget(protocolLabel);

    ui_serverAddressEdit = new QLineEdit(onlineWidget);
    ui_serverAddressEdit->setPlaceholderText("ramses.rxlab.io/yourAccountName");
    adressLayout->addWidget(ui_serverAddressEdit);

    QLabel *sslLabel = new QLabel(tr("Secure connexion"), this);
    onlineLayout->addWidget(sslLabel, 1, 0);

    ui_sslCheckBox = new QCheckBox("Use SSL", this);
    ui_sslCheckBox->setChecked(true);
    onlineLayout->addWidget(ui_sslCheckBox, 1, 1);

    QLabel *updateFreqLabel = new QLabel(tr("Update every"), this);
    onlineLayout->addWidget(updateFreqLabel, 2, 0);

    ui_updateFreqSpinBox = new QSpinBox(this);
    ui_updateFreqSpinBox->setMinimum(15);
    ui_updateFreqSpinBox->setMaximum(600);
    ui_updateFreqSpinBox->setValue(60);
    ui_updateFreqSpinBox->setSuffix(" seconds");
    onlineLayout->addWidget(ui_updateFreqSpinBox, 2, 1);

    QLabel *timeOutLabel = new QLabel(tr("Server timeout"), this);
    onlineLayout->addWidget(timeOutLabel, 3, 0);

    ui_timeoutSpinBox = new QSpinBox(this);
    ui_timeoutSpinBox->setMinimum(1);
    ui_timeoutSpinBox->setMaximum(10);
    ui_timeoutSpinBox->setValue(3);
    ui_timeoutSpinBox->setSuffix(" seconds");
    onlineLayout->addWidget(ui_timeoutSpinBox, 3, 1);

    ui_createButton = new QPushButton(tr("Create and log in"));
    ui_createButton->setIcon(QIcon(":/icons/apply"));
    mainLayout->addWidget(ui_createButton);

    mainLayout->addStretch();

#ifdef QT_DEBUG
    ui_fileSelector->setPath("/home/duduf/Documents/test.ramses");
    ui_shortNameEdit->setText("Duf");
    ui_npassword1Edit->setText("pass");
    ui_npassword2Edit->setText("pass");
#endif
}

void DatabaseCreateWidget::connectEvents()
{
    connect(ui_fileSelector, &DuQFFolderSelectorWidget::pathChanged, this, &DatabaseCreateWidget::checkPath);
    connect(ui_createButton, &QPushButton::clicked, this, &DatabaseCreateWidget::createDB);
}
