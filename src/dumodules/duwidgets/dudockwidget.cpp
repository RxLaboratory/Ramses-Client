#include "dudockwidget.h"

#include <QVariant>

#include "duwidgets/duicon.h"
#include "qtimer.h"

DuDockWidget::DuDockWidget(QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(parent, flags)
{

}

DuDockWidget::DuDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(title, parent, flags)
{
    ui_dockTitleWidget = new DuDockTitleWidget(title, this);
    this->setTitleBarWidget(ui_dockTitleWidget);
}

DuDockWidget::DuDockWidget(const QString &title, const QString &icon, QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(title, parent, flags)
{
    this->setWindowIcon(DuIcon(icon));

    ui_dockTitleWidget = new DuDockTitleWidget(title, this);
    ui_dockTitleWidget->setIcon(icon);
    this->setTitleBarWidget(ui_dockTitleWidget);
}

void DuDockWidget::setWidget(QWidget *w)
{
    if (_autoDeleteWidget) {
        auto previousWidget = this->widget();
        if (previousWidget)
            previousWidget->deleteLater();
    }

    QDockWidget::setWidget(w);
}

void DuDockWidget::setAutoDeleteWidget(bool autoDelete)
{
    _autoDeleteWidget = autoDelete;
}

void DuDockWidget::deleteWidget()
{
    if (_autoDeleteWidget) {
        auto w = this->widget();
        if (w) // Give it a sec for the animation
            QTimer::singleShot(500, w, &QWidget::deleteLater);
    }
}

void DuDockWidget::closeEvent(QCloseEvent *e)
{
    if (_autoDeleteWidget) {
        auto w = this->widget();
        if (w) // Give it a sec for the animation
            QTimer::singleShot(500, w, &QWidget::deleteLater);
    }
    QDockWidget::closeEvent(e);
}
