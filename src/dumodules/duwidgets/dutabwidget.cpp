#include "dutabwidget.h"

#include <QTabBar>

#include "duapp/duui.h"

DuTabWidget::DuTabWidget(QWidget *parent):
    QTabWidget(parent)
{
    this->setIconSize(QSize(16,16));
    setIconOnly(true);
    tabBar()->setAutoHide(true);
    setTabPosition(QTabWidget::West);
}

void DuTabWidget::setIconOnly(bool onlyIcon)
{
    if (onlyIcon) DuUI::setCSS(this,
                     "QTabBar::tab {width:16px; height:16px;}"
                     );
    else DuUI::setCSS( this, "" );
}
