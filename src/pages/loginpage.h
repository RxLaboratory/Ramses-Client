#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QScrollBar>
#include <QSysInfo>

#include "duqf-widgets/duqfservercombobox.h"
#include "duqf-widgets/duqfsslcheckbox.h"
#include "duqf-utils/simplecrypt.h"

#include "ui_loginpage.h"

#include "daemon.h"
#include "ramses.h"

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

signals:
    void serverSettings();

private slots:
    void loggedIn(RamUser *user);
    void loggedOut();
    void dbiData(QJsonObject data);

    void loginButton_clicked();
    void serverSettingsButton_clicked();
    void serverAddressChanged(QString address);

    void toggleSaveUsername(bool enabled);
    void toggleSavePassword(bool enabled);

private:
    void setupUi();
    void connectEvents();

    QWidget *ui_loginWidget;
    DuQFServerComboBox *ui_serverBox;
    DuQFSSLCheckbox *ui_sslBox;
    QLineEdit *ui_usernameEdit;
    QCheckBox *ui_saveUsername;
    QLineEdit *ui_passwordEdit;
    QCheckBox *ui_savePassword;
    QLabel *ui_capsLockLabel;
    QPushButton *ui_loginButton;
    QLabel *ui_connectionStatusLabel;

    QString m_hashedPassword;

    Ramses *_ramses;
    QTimer *_failedTimer;
    QTimer *_uiTimer;
    int _failedAttempts;
    void freeze();
    void unFreeze();
    void updateFreeze();
};

#endif // LOGINPAGE_H
