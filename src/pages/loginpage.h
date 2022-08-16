#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QScrollBar>
#include <QSysInfo>

#include "ramses.h"

#include "databasecreatewidget.h"

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

private slots:
    void createDatabase();
    void updateDatabaseRecentList();
    void databaseChanged(int i);

    void loggedIn(RamUser *user);
    void loggedOut(QString reason);

    void loginButton_clicked();

    void toggleSaveUsername(bool enabled);
    void toggleSavePassword(bool enabled);

private:
    void setupUi();
    void connectEvents();

    QWidget *ui_loginWidget;
    QComboBox *ui_dataBaseBox;
    QPushButton *ui_createDBButton;
    QPushButton *ui_openDBButton;
    QPushButton *ui_settingsDBButton;
    QLineEdit *ui_keyEdit;
    QLineEdit *ui_usernameEdit;
    QCheckBox *ui_saveUsername;
    QLineEdit *ui_passwordEdit;
    QCheckBox *ui_savePassword;
    QLabel *ui_capsLockLabel;
    QPushButton *ui_loginButton;
    QLabel *ui_connectionStatusLabel;
    QLabel *ui_waitLabel;
    DatabaseCreateWidget *ui_databaseCreateWidget = nullptr;

    QString m_hashedPassword;

    Ramses *m_ramses;
    QTimer *m_failedTimer;
    QTimer *m_uiTimer;
    int m_failedAttempts;
    void freeze();
    void unFreeze();
    void updateFreeze();
};

#endif // LOGINPAGE_H
