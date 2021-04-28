#include "applicationlistmanagerwidget.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        new ApplicationEditWidget(),
        "Users",
        parent )
{

}

void ApplicationListManagerWidget::createObject()
{
    Ramses::instance()->createApplication();
}
