#include "applicationlistmanagerwidget.h"

#include "ramses.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        new ApplicationEditWidget(),
        "Applications",
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void ApplicationListManagerWidget::createObject()
{
    RamApplication *a = new RamApplication(
                "NEW",
                "New Application");
    Ramses::instance()->applications()->append(a);
    editObject(a);
}
