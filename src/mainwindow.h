#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMouseEvent>
#include <QLabel>
#include <QToolButton>
#include <QDesktopServices>
#include <QtDebug>
#include <QSettings>
#include <QProcess>
#include <QMessageBox>
#include <QSystemTrayIcon>

#include "duqf-widgets/dumainwindow.h"
#include "projecteditwidget.h"

#include "duqf-widgets/settingswidget.h"
#include "duqf-utils/utils.h"
#include "duqf-widgets/dudocktitlewidget.h"
#include "progresspage.h"
#include "ramproject.h"
#include "databaseeditwidget.h"

class MainWindow : public DuMainWindow
{
    Q_OBJECT

public:

    enum Page {
        Home = 0,
        UserProfile = 1,
        Admin = 2,
        PipeLine = 3,
        Assets = 4,
        Shots = 5,
        Schedule = 6,
        Progress = 7
    };

    explicit MainWindow(const QCommandLineParser &cli, QWidget *parent = nullptr);

    void setPropertiesDockWidget(QWidget *w, QString title = "Properties", QString icon = ":/icons/asset");

    // Used by frameless window
    void setMaximizedState(bool maximized);

public slots:
    void hidePropertiesDock();

signals:
    void hideTriggered();
    void showTriggered();

private:
    void setupActions();
    void setupUi();
    void setupToolBar();
    void setupSysTray();
    void setupDocks();
    void connectEvents();
    void connectShortCuts();

    // ==== ACTIONS ====

    DuAction *m_actionConsole;
    DuAction *m_actionLogIn;
    DuAction *m_actionLogOut;
    DuAction *m_actionUserProfile;
    DuAction *m_actionAdmin;
    DuAction *m_actionUserFolder;
    DuAction *m_actionPipeline;
    DuAction *m_actionAssets;
    DuAction *m_actionShots;
    DuAction *m_actionSchedule;
    DuAction *m_actionStatistics;
    DuAction *m_actionTimeline;
    DuAction *m_actionSetOnline;
    DuAction *m_actionSetOffline;
    DuAction *m_actionDatabaseSettings;
    DuAction *m_actionSettings;
    DuAction *m_actionSync;
    DuAction *m_actionFullSync;
    DuAction *m_projectAction;
    DuAction *m_stepsAction;
    DuAction *m_pipeFilesAction;
    DuAction *m_assetGroupAction;
    DuAction *m_assetListAction;
    DuAction *m_sequenceAction;
    DuAction *m_shotListAction;
    DuAction *m_fileAdminAction;
    DuAction *m_filePreprodAction;
    DuAction *m_fileProdAction;
    DuAction *m_filePostProdAction;
    DuAction *m_fileAssetsAction;
    DuAction *m_fileShotsAction;
    DuAction *m_fileOutputAction;
    DuAction *m_fileUserAction;
    DuAction *m_fileVersionsAction;
    DuAction *m_fileTrashAction;
    DuAction *m_actionShowHide;

    // ==== WIDGETS ====

    QStackedWidget *ui_mainStack;
    QWidget *ui_mainPage;

    // ==== Docks ====

    DuDockWidget *ui_settingsDock;
    DuDockWidget *ui_statsDockWidget;
    DuDockTitleWidget *ui_statsTitle;
    DuDockWidget *ui_consoleDockWidget;
    DuDockWidget *ui_propertiesDockWidget;
    DuDockWidget *ui_timelineDockWidget;
    DuDockWidget *ui_stepsDockWidget;
    DuDockWidget *ui_pipeFileDockWidget;
    ProjectEditWidget *ui_projectEditWiget;
    DuDockWidget *ui_projectDockWidget;
    DuDockWidget *ui_assetGroupsDockWidget;
    DuDockWidget *ui_assetsDockWidget;
    DuDockWidget *ui_sequencesDockWidget;
    DuDockWidget *ui_shotsDockWidget;
    DuDockTitleWidget *ui_propertiesTitle;

    // ========= RxOT UI ==============

    /**
     * @brief raise Tries to (force) raise the window
     */
    void raise();

    QSystemTrayIcon *trayIcon;
    ProgressPage *progressPage;
    bool m_maximized;

    // ToolBar Menus
    QAction *ui_projectSelectorAction;

    QToolButton *ui_pipelineButton;
    QAction *ui_pipelineMenuAction;

    QToolButton *ui_assetsButton;
    QAction *ui_assetMenuAction;

    QToolButton *ui_shotsButton;
    QAction *ui_shotMenuAction;

    QToolButton *ui_scheduleButton;
    QAction *ui_scheduleMenuAction;

    QToolButton *ui_filesButton;
    QAction *ui_filesMenuAction;

    QToolButton *ui_userButton;
    QAction *ui_userMenuAction;

    QToolButton *ui_databaseButton;
    QAction *ui_databaseMenuAction;

    SettingsWidget *ui_adminPage;

    DatabaseEditWidget *ui_databaseEditWidget = nullptr;

    int m_currentPageIndex = 0;

    QMetaObject::Connection _currentUserConnection;

    bool m_shiftPressed = false;

    /**
     * @brief m_showUpdateAlerts is set to true after the auto-update check during startup
     */
    bool m_showUpdateAlerts = false;

private slots:
    void bugReport();
    void forum();
    void chat();
    void doc();
    void donate();
    void about();
    void trayClicked(QSystemTrayIcon::ActivationReason reason);
    void showHide();
    void askBeforeClose();
    void maximize(bool m);

    void pageChanged(int i);
    void setOfflineAction();
    void setOnlineAction();
    void databaseSettingsAction();
    void home();
    void userProfile();
    void admin();
    void pipeline();
    void shots();
    void assets();
    void schedule();
    void currentUserChanged();
    void currentProjectChanged(RamProject *project);
    void freezeUI(bool f = true);
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);
    void finishSync();
    void startSync();

    void revealAdminFolder();
    void revealPreProdFolder();
    void revealProdFolder();
    void revealPostProdFolder();
    void revealAssetsFolder();
    void revealShotsFolder();
    void revealOutputFolder();
    void revealUserFolder();
    void revealVersionsFolder();
    void revealTrashFolder();

    void onQuit();

protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *key) override;
    void keyReleaseEvent(QKeyEvent *key) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void paintEvent(QPaintEvent *e) override;

private:
    bool m_readyToClose = false;
    bool m_closing = false;
};
#endif // MAINWINDOW_H
