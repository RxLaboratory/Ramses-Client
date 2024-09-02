#ifndef DUMAINWINDOW_H
#define DUMAINWINDOW_H

#include <QVBoxLayout>
#include <QMainWindow>
#include <QCommandLineParser>
#include <QLabel>

#include "src/qgoodcentralwidget.h"
#include "src/qgoodwindow.h"

#include "duqf-widgets/dudockwidget.h"


/**
 * @brief The DuDockEventFilter class filters events on QDockWidgets to animate them
 */
class DuDockEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit DuDockEventFilter(QObject *parent = nullptr):
        QObject(parent) {}
    /**
     * @brief suspend Set to true when in an interaction with the dock
     * and you don't want the event filter to process events.
     * @param i
     */
    void suspend(bool s) { m_suspended = s; };
signals:
    void dockVisibilityChanged(QDockWidget *, bool);
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
private:
    bool m_suspended = false;
};

/**
 * @brief The DuMainWindow class
 * @version 1.5.0
 * Use DuDockEventFilter class
 * == Version history  ==
 * 1.4.1
 * Frameless window using QGoodWindow
 * add hideAllDocks method
 * performance fixes
 */
class DuMainWindow : public QGoodWindow
{
    Q_OBJECT
public:

#ifdef _WIN32
    friend class QWinWidget;
#endif

    explicit DuMainWindow(QWidget *parent = nullptr);

    // Override to set the title on the toolbar
    void setWindowTitle(const QString &title);
    // Override to make them animatable
    void addDockWidget(Qt::DockWidgetArea area, DuDockWidget *dockwidget);
    const QVector<DuDockWidget*> &docks() const;

    // Hides the title on the toolbar
    void hideTitle();
    void hideAllDocks();

protected:
    void closeEvent(QCloseEvent *event) override;
    void setStyle();

    QToolBar *ui_mainToolBar;
    QBoxLayout *ui_mainLayout;
    QLabel *ui_titleLabel;

private slots:
    void setDockVisible(QDockWidget *w, bool visible = true);
    void setDockIcons();

private:
    void setupActions();
    void setupUi();
    void setupTitleBar();

    QVector<DuDockWidget*> ui_docks;
    QMainWindow *ui_centralMainWidget;
    QGoodCentralWidget *ui_goodCentralWidget;

    bool m_toolBarClicked;
    QPoint m_dragPosition;
    bool m_maximized = false;
    DuDockEventFilter *m_dockEventFilter;
};

#endif // DUMAINWINDOW_H
