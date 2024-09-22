#ifndef SERVERWIZARD_H
#define SERVERWIZARD_H

#include "wizards/loginwizardpage.h"
#include <QWizard>

class ServerWizard : public QWizard
{
    Q_OBJECT
public:
    enum PageId {
        LoginPage,
    };

    ServerWizard(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

private:
    void setupUi();
    void connectEvents();

    LoginWizardPage *ui_loginPage;
};

#endif // SERVERWIZARD_H
