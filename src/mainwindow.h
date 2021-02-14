#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"
#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/settingswidget.h"
#include "duqf-widgets/aboutdialog.h"

#include "serversettingswidget.h"
#include "loginpage.h"
#include "userprofilepage.h"
#include "dbinterface.h"
#include "duqf-utils/utils.h"

#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>
#include <QMenu>
#include <QDesktopServices>
#include <QtDebug>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>

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
    QSettings settings;
    SettingsWidget *settingsWidget;
    QLabel *title;
    QMenu *helpMenu;

    QToolButton *userButton;
    QToolButton *networkButton;

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

    void log(QString m, LogUtils::LogType type);

    void serverSettings();
    void loginAction();
    void logoutAction();
    void userProfile();
    void userButton_clicked();
    void networkButton_clicked();
    void loggedIn();
    void loggedOut();
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);

protected:
    void closeEvent(QCloseEvent *event);
    bool eventFilter(QObject *obj, QEvent *event);

};
#endif // MAINWINDOW_H
