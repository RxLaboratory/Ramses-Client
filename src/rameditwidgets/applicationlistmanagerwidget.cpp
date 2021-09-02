#include "applicationlistmanagerwidget.h"

#include "ramses.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        new ApplicationEditWidget(),
        "Applications",
        QIcon(":icons/application"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *ApplicationListManagerWidget::createObject()
{
    RamApplication *a = new RamApplication(
                "NEW",
                "New Application");
    Ramses::instance()->applications()->append(a);
    editObject(a);
    return a;
}
