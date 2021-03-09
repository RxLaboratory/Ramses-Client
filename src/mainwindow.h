#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/settingswidget.h"
#include "duqf-widgets/appearancesettingswidget.h"
#include "duqf-widgets/aboutdialog.h"

#include "serversettingswidget.h"
#include "daemonsettingswidget.h"
#include "loginpage.h"
#include "userprofilepage.h"
#include "usersmanagerwidget.h"
#include "projectsmanagerwidget.h"
#include "templatestepsmanagerwidget.h"
#include "templateassetgroupsmanagerwidget.h"
#include "stepsmanagerwidget.h"
#include "assetgroupsmanagerwidget.h"
#include "statesmanagerwidget.h"
#include "dbinterface.h"
#include "daemon.h"
#include "projectselectorwidget.h"
#include "localsettingswidget.h"
#include "duqf-utils/utils.h"
#include "duqf-widgets/toolbarspacer.h"

#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QDesktopServices>
#include <QtDebug>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QSystemTrayIcon>

class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStringList args, QWidget *parent = nullptr);

private:
    // ========= RxOT UI ==============
    /**
     * @brief initUi Called once to build the default RxOT UI
     */
    void duqf_initUi();
    /**
     * @brief duqf_setStyle Called once to set the UI Style after all ui have been created
     */
    void duqf_setStyle();
    /**
     * @brief Is the tool bar currently clicked or not
     */
    bool duqf_toolBarClicked;
    /**
     * @brief Drag position
     * Used for drag n drop feature
     */
    QPoint duqf_dragPosition;
    QToolButton *duqf_maximizeButton;
    QToolButton *duqf_settingsButton;
    AboutDialog *duqf_aboutDialog;
    QAction *duqf_actionShowHide;
    QSettings settings;
    SettingsWidget *settingsWidget;
    QLabel *title;
    QMenu *helpMenu;

    SettingsWidget *adminPage;
    SettingsWidget *projectSettingsPage;
    QMenu *userMenu;
    QToolButton *userButton;
    QToolButton *networkButton;
    QToolButton *refreshButton;

    QMetaObject::Connection _currentUserConnection;

private slots:
    void duqf_maximize(bool max);
    void duqf_maximize();
    void duqf_bugReport();
    void duqf_forum();
    void duqf_chat();
    void duqf_doc();
    void duqf_settings(bool checked = true);
    void duqf_closeSettings();
    void duqf_reinitSettings();
    void duqf_about();
    void duqf_trayClicked(QSystemTrayIcon::ActivationReason reason);
    void duqf_showHide();
    void duqf_askBeforeClose();

    void log(QString m, LogUtils::LogType type);

    void pageChanged(int i);
    void serverSettings();
    void loginAction();
    void logoutAction();
    void home();
    void userProfile();
    void revealUserFolder();
    void admin(bool show = true);
    void closeAdmin();
    void projectSettings(bool show = true);
    void closeProjectSettings();
    void networkButton_clicked();
    void loggedIn();
    void loggedOut();
    void currentUserChanged();
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

};
#endif // MAINWINDOW_H
