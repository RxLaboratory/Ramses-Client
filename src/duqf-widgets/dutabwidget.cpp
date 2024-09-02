#include "dutabwidget.h"
#include "duqf-app/duui.h"

DuTabWidget::DuTabWidget(QWidget *parent):
    QTabWidget(parent)
{
    this->setIconSize(QSize(16,16));
    setIconOnly(true);
}

void DuTabWidget::setIconOnly(bool onlyIcon)
{
    if (onlyIcon) DuUI::setCSS(this,
                     "QTabBar::tab {width: @medium-icon; height: @medium-icon;}"
                     );
    else DuUI::setCSS( this,"" );
}
