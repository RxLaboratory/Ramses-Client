#ifndef DUMAINWINDOW_H
#define DUMAINWINDOW_H

#include <QVBoxLayout>
#include <QMainWindow>
#include <QCommandLineParser>
#include <QLabel>
#include <QGraphicsColorizeEffect>

#include "src/qgoodcentralwidget.h"
#include "src/qgoodwindow.h"

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
 * @version 1.6.0
 * - Use left/center/right toolbars
 *   instead of a single main toolbar
 * - Expose ui_centralWidget
 *   instead of a default layout
 * - Add updateWindow()
 * == Version history  ==
 * 1.5.0
 * - Use DuDockEventFilter class
 * 1.4.1
 * - Frameless window using QGoodWindow
 * - add hideAllDocks method
 * - performance fixes
 */
class DuMainWindow : public QGoodWindow
{
    Q_OBJECT
public:

    explicit DuMainWindow(QWidget *parent = nullptr);

    // Override to set the title on the toolbar
    void setWindowTitle(const QString &title);

    // Override to make them animatable
    void addDockWidget(Qt::DockWidgetArea area, QDockWidget *dockwidget);
    const QVector<QDockWidget *> &docks() const;

    int execDialog(QDialog *dialog) const;

public slots:
    /**
     * @brief hideTitle Hides the title on the title bar
     */
    void hideTitle();
    /**
     * @brief hideAllDocks Hides all the QDockWidgets
     */
    void hideAllDocks();
    /**
     * @brief updateWindow Must be called to update the title bar
     * when widgets are added/removed or their visibility is changed.
     * Call it if the right/center/left toolbars are modified.
     * Note that it is automatically called with setStyle().
     */
    void updateWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void setStyle();

    QWidget *ui_centralWidget;

    QToolBar *ui_leftToolBar;
    QToolBar *ui_centerToolBar;
    QToolBar *ui_rightToolBar;

private slots:
    void setDockVisible(QDockWidget *w, bool visible = true);
    void setDockIcons();

private:
    void setupActions();
    void setupUi();
    void setupTitleBar();

    QVector<QDockWidget*> ui_docks;
    QMainWindow *ui_centralMainWidget;
    QGoodCentralWidget *ui_goodCentralWidget;
    QLabel *ui_titleLabel;

    bool m_toolBarClicked;
    QPoint m_dragPosition;
    bool m_maximized = false;
    DuDockEventFilter *m_dockEventFilter;

    QGraphicsColorizeEffect *_colorizer;
};

#endif // DUMAINWINDOW_H
