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
    QString uuid() const { return _uuid; }

private:
    void setupUi();
    void connectEvents();

    ServerEditWidget *ui_serverWidget;
    DuLineEdit *ui_emailEdit;
    DuLineEdit *ui_passwordEdit;

    QString _uuid;
};

#endif // LOGINWIZARDPAGE_H
