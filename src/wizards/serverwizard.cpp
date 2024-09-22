#include "serverwizard.h"

ServerWizard::ServerWizard(QWidget *parent, Qt::WindowFlags flags):
    QWizard(parent, flags)
{
    setupUi();
    connectEvents();
}

void ServerWizard::setupUi()
{
    ui_loginPage = new LoginWizardPage(true, this);
    setPage(LoginPage, ui_loginPage );
}

void ServerWizard::connectEvents()
{

}
