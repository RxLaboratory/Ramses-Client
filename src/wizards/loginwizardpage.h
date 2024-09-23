#ifndef LOGINWIZARDPAGE_H
#define LOGINWIZARDPAGE_H

#include <QWizardPage>


#include "servereditwidget.h"
#include "duwidgets/dulineedit.h"

class LoginWizardPage : public QWizardPage
{
    Q_OBJECT
public:
    LoginWizardPage(bool mustBeAdmin, QWidget *parent = nullptr);
    bool validatePage() override;
    QString uuid() const { return _uuid; }
    QString serverAddress() const { return ui_serverWidget->address(); }

signals:
    void uuidChanged(const QString &);

private:
    void setupUi();
    void connectEvents();

    ServerEditWidget *ui_serverWidget;
    DuLineEdit *ui_emailEdit;
    DuLineEdit *ui_passwordEdit;

    QString _uuid;
    bool _mustBeAdmin;

    Q_PROPERTY(QString uuid READ uuid NOTIFY uuidChanged FINAL)
};

#endif // LOGINWIZARDPAGE_H
