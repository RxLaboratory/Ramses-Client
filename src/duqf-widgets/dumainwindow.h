#ifndef DUMAINWINDOW_H
#define DUMAINWINDOW_H

#include <QVBoxLayout>
#include <QMainWindow>
#include <QCommandLineParser>
#include <QLabel>

#include "duqf-widgets/duaction.h"
#include "duqf-widgets/dudockwidget.h"

/**
 * @brief The DuMainWindow class
 * @version 1.1.0 Removed addWindowButtons and setMaximizedState (not frameless anymore)
 */
class DuMainWindow : public QMainWindow
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

signals:
    void minimizeTriggered();
    void maximizeTriggered(bool);
    void closeTriggered();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

    void setStyle();

    QToolBar *ui_mainToolBar;
    QVBoxLayout *ui_mainLayout;

    QLabel *ui_titleLabel;
    DuAction *m_minimizeAction;
    DuAction *m_maximizeAction;
    DuAction *m_closeAction;

private slots:
    void setDockVisible(DuDockWidget *w, bool visible = true);
    void setDockIcons();

private:
    void setupActions();
    void setupUi();
    void setupToolBar();

    QVector<DuDockWidget*> ui_docks;

    bool m_toolBarClicked;
    QPoint m_dragPosition;
    DuDockWidget *m_dockInteraction = nullptr;
    bool m_maximized = false;
};

#endif // DUMAINWINDOW_H
