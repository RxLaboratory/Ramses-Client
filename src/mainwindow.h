#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include "ui_mainwindow.h"
#include "duqf-app/app-version.h"
#include "duqf-app/app-style.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/settingswidget.h"
#include "duqf-widgets/appearancesettingswidget.h"
#include "duqf-widgets/aboutdialog.h"
#include "duqf-utils/utils.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/duqflogtoolbutton.h"
#include "duqf-widgets/duqfupdatedialog.h"
#include "duqf-widgets/duqfupdatesettingswidget.h"
#include "duqf-widgets/duqfdocktitle.h"
#include "duqf-widgets/duqfautosizetoolbutton.h"

#include "serversettingswidget.h"
#include "daemonsettingswidget.h"
#include "loginpage.h"
#include "userprofilepage.h"
#include "userlistmanagerwidget.h"
#include "projectlistmanagerwidget.h"
#include "pipeline-editor/pipelinewidget.h"
#include "templatesteplistmanagerwidget.h"
#include "templateassetgrouplistmanagerwidget.h"
#include "statelistmanagerwidget.h"
#include "filetypelistmanagerwidget.h"
#include "applicationlistmanagerwidget.h"
#include "schedulemanagerwidget.h"
#include "docks/statisticswidget.h"
#include "dbinterface.h"
#include "daemon.h"
#include "ramloader.h"
#include "projectselectorwidget.h"
#include "localsettingswidget.h"
#include "progresspage.h"
#include "processmanager.h"
#include "progressbar.h"
#include "pages/projectpage.h"
#include "pages/installpage.h"

#include "rameditwidgets/itemtablemanagerwidget.h"


class MainWindow : public QMainWindow, private Ui::MainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QStringList args, QWidget *parent = nullptr);

    void setPropertiesDockWidget(QWidget *w, QString title = "Properties", QString icon = ":/icons/asset");

private:
    // ========= RxOT UI ==============
    /**
     * @brief duqf_checkUpdate Called once to check if an update is available
     */
    void duqf_checkUpdate();
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
    QSystemTrayIcon *trayIcon;
    SettingsWidget *settingsWidget;
    QLabel *title;
    QMenu *helpMenu;
    ProgressPage *progressPage;
    QDockWidget *ui_statsDockWidget;
    QDockWidget *ui_consoleDockWidget;
    QDockWidget *ui_propertiesDockWidget;
    DuQFDockTitle *ui_statsTitle;
    DuQFDockTitle *ui_propertiesTitle;

    QMenu *ui_userMenu;
    DuQFAutoSizeToolButton *ui_userButton;
    DuQFAutoSizeToolButton *ui_networkButton;
    QToolButton *ui_refreshButton;
    QToolButton *ui_consoleButton;
    int m_currentPageIndex = 0;

    QMetaObject::Connection _currentUserConnection;

    bool m_shiftPressed = false;

    /**
     * @brief m_showUpdateAlerts is set to true after the auto-update check during startup
     */
    bool m_showUpdateAlerts = false;

private slots:
    void duqf_maximize(bool max);
    void duqf_maximize();
    void duqf_bugReport();
    void duqf_forum();
    void duqf_chat();
    void duqf_doc();
    void duqf_donate();
    void duqf_settings(bool checked = true);
    void duqf_closeSettings();
    void duqf_reinitSettings();
    void duqf_about();
    void duqf_trayClicked(QSystemTrayIcon::ActivationReason reason);
    void duqf_showHide();
    void duqf_askBeforeClose();
    void duqf_updateAvailable(QJsonObject updateInfo);

    void log(DuQFLog m);

    void pageChanged(int i);
    void serverSettings();
    void loginAction();
    void logoutAction();
    void home();
    void userProfile();
    void revealUserFolder();
    void admin(bool show = true);
    void projectSettings(bool show = true);
    void pipeline(bool show = true);
    void shots(bool show = true);
    void assets(bool show = true);
    void schedule(bool show = true);
    void install(bool show = true);
    void networkButton_clicked();
    void loggedIn();
    void loggedOut();
    void currentUserChanged();
    void currentProjectChanged(RamProject *project);
    void freezeUI(bool f = true);
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *key) override;
    void keyReleaseEvent(QKeyEvent *key) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
};
#endif // MAINWINDOW_H
