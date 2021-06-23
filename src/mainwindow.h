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
#include "userlistmanagerwidget.h"
#include "projectlistmanagerwidget.h"
#include "pipeline-editor/pipelinewidget.h"
#include "templatesteplistmanagerwidget.h"
#include "templateassetgrouplistmanagerwidget.h"
#include "steplistmanagerwidget.h"
#include "assetgrouplistmanagerwidget.h"
#include "assetlistmanagerwidget.h"
#include "shotlistmanagerwidget.h"
#include "sequencelistmanagerwidget.h"
#include "statelistmanagerwidget.h"
#include "filetypelistmanagerwidget.h"
#include "applicationlistmanagerwidget.h"
#include "pipefilelistmanagerwidget.h"
#include "dbinterface.h"
#include "daemon.h"
#include "ramloader.h"
#include "projectselectorwidget.h"
#include "localsettingswidget.h"
#include "progresspage.h"
#include "processmanager.h"
#include "progressbar.h"
#include "duqf-utils/utils.h"
#include "duqf-widgets/toolbarspacer.h"
#include "duqf-widgets/duqflogtoolbutton.h"
#include "rameditwidgets/assettablemanagerwidget.h"
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

    void addObjectDockWidget(ObjectDockWidget *w);

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
    QSystemTrayIcon *trayIcon;
    QSettings settings;
    SettingsWidget *settingsWidget;
    QLabel *title;
    QMenu *helpMenu;
    ProgressPage *progressPage;
    ProjectEditWidget *ui_currentProjectSettings;

    QMenu *userMenu;
    QToolButton *userButton;
    QToolButton *networkButton;
    QToolButton *refreshButton;
    int m_currentPageIndex = 0;

    QList<ObjectDockWidget *> _dockedObjects;

    QMetaObject::Connection _currentUserConnection;

    bool m_shiftPressed = false;

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

    void log(DuQFLog m);

    void dockWidgetDestroyed(QObject *dockObj);

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
