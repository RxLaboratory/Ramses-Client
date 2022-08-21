#include "applicationlistmanagerwidget.h"

#include "ramses.h"

ApplicationListManagerWidget::ApplicationListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->applications(),
        "Applications",
        QIcon(":icons/application"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    m_listEditWidget->setSortMode(RamObjectList::Name);
    m_listEditWidget->sort();
}

RamApplication *ApplicationListManagerWidget::createObject()
{
    RamApplication *a = new RamApplication(
                "NEW",
                "New Application");
    Ramses::instance()->applications()->append(a);
    a->edit();
    return a;
}
