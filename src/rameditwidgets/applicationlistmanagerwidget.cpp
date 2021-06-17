#include "applicationlistmanagerwidget.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        new ApplicationEditWidget(),
        "Applications",
        parent )
{
    this->setContainingType(RamObject::Application);
}

void ApplicationListManagerWidget::createObject()
{
    Ramses::instance()->createApplication();
}
