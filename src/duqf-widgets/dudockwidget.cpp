#include "dudockwidget.h"

#include <QVariant>

#include "duqf-app/duui.h"

DuDockWidget::DuDockWidget(QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(parent, flags)
{

}

DuDockWidget::DuDockWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags):
    QDockWidget(title, parent, flags)
{

}

void DuDockWidget::setWidget(QWidget *w)
{
    w->setProperty("class", "duDock");
    DuUI::addCSS(w, "duDock");

    QDockWidget::setWidget(w);
}
