#include "applicationlistmanagerwidget.h"

#include "ramses.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        new ApplicationEditWidget(),
        "Applications",
        parent )
{

}

void ApplicationListManagerWidget::createObject()
{
    Ramses::instance()->createApplication();
}
