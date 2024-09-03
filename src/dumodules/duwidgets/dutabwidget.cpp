#include "dutabwidget.h"
#include "duapp/duui.h"

DuTabWidget::DuTabWidget(QWidget *parent):
    QTabWidget(parent)
{
    this->setIconSize(QSize(16,16));
    setIconOnly(true);
}

void DuTabWidget::setIconOnly(bool onlyIcon)
{
    if (onlyIcon) DuUI::setCSS(this,
                     "QTabBar::tab {width:16px; height:16px;}"
                     );
    else DuUI::setCSS( this,"" );
}
