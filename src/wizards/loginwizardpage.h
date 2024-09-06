#ifndef LOGINWIZARDPAGE_H
#define LOGINWIZARDPAGE_H

#include <QWizardPage>


#include "servereditwidget.h"
#include "duwidgets/dulineedit.h"

class LoginWizardPage : public QWizardPage
{
public:
    LoginWizardPage(QWidget *parent = nullptr);
    bool validatePage() override;

private:
    void setupUi();
    void connectEvents();

    ServerEditWidget *ui_serverWidget;
    DuLineEdit *ui_usernameEdit;
    DuLineEdit *ui_passwordEdit;
};

#endif // LOGINWIZARDPAGE_H
