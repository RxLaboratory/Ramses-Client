#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QScrollBar>
#include <QSysInfo>

#include "ramses.h"

#include "databasecreatewidget.h"
#include "databaseeditwidget.h"

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);

private slots:
    void createDatabase();
    void openDatabase();
    void editDatabase();

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

    QStackedWidget *ui_stackWiget;
    QWidget *ui_loginWidget;
    QWidget *ui_bigButtonWidget;
    QPushButton *ui_bigOpenButton;
    QPushButton *ui_bigCreateButton;
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
    DatabaseEditWidget *ui_databaseEditWidget = nullptr;

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
