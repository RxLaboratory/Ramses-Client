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

#include "duwidgets/duiconwidget.h"
#include "duwidgets/dumainwindow.h"
#include "landingpagewidget.h"
#include "projecteditwidget.h"

#include "duwidgets/settingswidget.h"
#include "duwidgets/duaction.h"
#include "duutils/utils.h"
#include "duwidgets/dudocktitlewidget.h"
#include "duwidgets/dudockwidget.h"
#include "ramproject.h"
#include "statemanager.h"
#include "localdatainterface.h"
#include "ramserverclient.h"

class MainWindow : public DuMainWindow
{
    Q_OBJECT

public:

    enum Page {
        LandingPage = 0,
        HomePage = 1,
        AdminPage = 2,
        PipeLinePage = 3,
        AssetsPage = 4,
        ShotsPage = 5,
        SchedulePage = 6,
    };

    enum Dock {
        SettingsDock,
        ConsoleDock,
        PropertiesDock,
        StatisticsDock,
        StepsDock,
        PipeFilesDock,
        AssetGroupsDock,
        AssetsDock,
        SequencesDock,
        ShotsDock,
        TimelineDock,
    };

    explicit MainWindow(const QCommandLineParser &cli, QWidget *parent = nullptr);

    void setPropertiesDockWidget(QWidget *w, QString title = "Properties", QString icon = ":/icons/asset", bool temporary = false);

public slots:
    void hidePropertiesDock();

signals:
    void hideTriggered();
    void showTriggered();

private:
    void setupActions();
    void setupUi();
    void setupToolBars();
    void setupSysTray();
    void setupDocks();

    bool updateCheck(bool force = false);

    void connectEvents();
    void connectShortCuts();

    // ==== ACTIONS ====

    DuAction *m_actionConsole;
    DuAction *m_actionCloseProject;
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
    DuAction *m_actionSettings;
    DuAction *m_actionSync;
    DuAction *m_actionFullSync;
    DuAction *m_actionUpdate;
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

    // ==== PAGES ====

    QStackedLayout *ui_mainStack;
    LandingPageWidget *ui_landingPage;
    Page m_currentPage = LandingPage;

    // ==== Docks ====

    DuDockWidget *ui_settingsDock;
    DuDockWidget *ui_statsDockWidget;
    DuDockWidget *ui_consoleDockWidget;
    DuDockWidget *ui_propertiesDockWidget;
    DuDockWidget *ui_timelineDockWidget;
    DuDockWidget *ui_stepsDockWidget;
    DuDockWidget *ui_pipeFileDockWidget;
    DuDockWidget *ui_projectDockWidget;
    DuDockWidget *ui_assetGroupsDockWidget;
    DuDockWidget *ui_assetsDockWidget;
    DuDockWidget *ui_sequencesDockWidget;
    DuDockWidget *ui_shotsDockWidget;

    bool _propertiesWidgetIsTemporary = false;

    /**
     * @brief raise Tries to (force) raise the window
     */
    void raise();

    QSystemTrayIcon *trayIcon;
    bool m_maximized;

    // ToolBar Menus

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

    DuMenu *ui_userMenu;
    QAction *ui_userMenuAction;

    DuMenu *ui_serverMenu;
    QAction *ui_serverMenuAction;

    DuIconWidget *ui_stateWidget;
    DuIconWidget *ui_ioWidget;

    SettingsWidget *ui_adminPage;

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

    void setOnlineAction();
    void setPage(Page p);
    void setDock(Dock d);
    void changeUserRole(RamUser::UserRole role);
    void ramsesReady();
    void changeState(StateManager::State s);
    void changeIOState(LocalDataInterface::State s);
    void changeServerState(RamServerClient::ClientStatus s);

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

    // Utils
    QColor normalStateIconColor();
    QColor warningStateIconColor();
    QColor focusStateIconColor();
};
#endif // MAINWINDOW_H
