#include "applicationmanagerwidget.h"

#include "duqf-widgets/duicon.h"
#include "ramses.h"

ApplicationManagerWidget::ApplicationManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->applications(),
        "Applications",
        DuIcon(":icons/application"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortMode(RamObject::Name);
    ui_listWidget->sort();
}

RamApplication *ApplicationManagerWidget::createObject()
{
    RamApplication *a = new RamApplication(
                "NEW",
                "New Application");
    a->edit();
    return a;
}
