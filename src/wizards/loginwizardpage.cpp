#include "loginwizardpage.h"

#include "duapp/duui.h"
#include "ramserverclient.h"

LoginWizardPage::LoginWizardPage(QWidget *parent):
    QWizardPage(parent)
{
    setupUi();
    connectEvents();
}

bool LoginWizardPage::validatePage()
{
    // Check server
    QString address = ui_serverWidget->address();
    QString test = address.split("/")[0];

    // Check if the host exists
    QHostInfo info = QHostInfo::fromName(test);
    if (info.error() != QHostInfo::NoError)
    {
        QMessageBox::warning(this,
                             tr("Wrong server"),
                             tr("Sorry, I can't connect to this server.\nPlease double check the address,\nand make sure you're connected to the internet.")
                             );
        return false;
    }

    // Check username and password
    QString username = ui_usernameEdit->text();
    if (username == "")
    {
        QMessageBox::warning(this,
                             tr("Missing username"),
                             tr("Please set your username to log in.")
                             );
        return false;
    }

    QString password = ui_passwordEdit->text();
    if (password == "")
    {
        QMessageBox::warning(this,
                             tr("Missing password"),
                             tr("Please set your password to log in.")
                             );
        return false;
    }

    // Set the server interface details
    auto client = RamServerClient::i();
    // Make sure we're offline
    client->setOffline();
    client->setServerAddress(address);
    client->setServerPort(ui_serverWidget->port());
    client->setSsl(ui_serverWidget->ssl());

    // Login and check admin rights
    QJsonObject response = client->login(username, password);
    if (!response.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Login failed"),
                             response.value("message").toString("Unknown error.")
                             );
        return false;
    }

    QJsonObject content = response.value("content").toObject();
    QString userDataStr = content.value("data").toString();
    QJsonDocument userDoc = QJsonDocument::fromJson(userDataStr.toUtf8());
    QJsonObject userData = userDoc.object();
    QString userRole = userData.value(RamUser::KEY_UserRole).toString(RamUser::ENUMVALUE_Standard);
    if (userRole != RamUser::ENUMVALUE_Admin) {
        QMessageBox::warning(this,
                             tr("Insuficient rights"),
                             tr("You must be a server administrator to create a team projects.\nPlease contact an administrator of this server.")
                             );
        return false;
    }

    return true;
}

void LoginWizardPage::setupUi()
{
    this->setTitle(tr("Connection"));
    this->setSubTitle(tr("Connect to a server and sign in."));

    auto layout = DuUI::createBoxLayout(Qt::Vertical);
    DuUI::centerLayout(layout, this);
    layout->setSpacing(16);

    ui_serverWidget = new ServerEditWidget(this);
    layout->addWidget(ui_serverWidget);

    auto loginLayout = DuUI::addFormLayout(layout);

    ui_usernameEdit = new DuLineEdit(this);
    ui_usernameEdit->setPlaceholderText(tr("Username"));
    loginLayout->addRow(tr("Username"), ui_usernameEdit);

    ui_passwordEdit = new DuLineEdit(this);
    ui_passwordEdit->setEchoMode(QLineEdit::Password);
    ui_passwordEdit->setPlaceholderText(tr("Password"));
    loginLayout->addRow(tr("Password"), ui_passwordEdit);
}

void LoginWizardPage::connectEvents()
{

}

