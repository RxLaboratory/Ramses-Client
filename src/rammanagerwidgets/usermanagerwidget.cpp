#include "usermanagerwidget.h"

#include "duwidgets/duicon.h"
#include "ramses.h"

UserManagerWidget::UserManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->users(),
        "Users",
        DuIcon(":icons/user"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "Ramses" << "Removed";
    ui_listWidget->setDontRemoveShortNameList(dontRemove);
    ui_listWidget->setSortable(false);
    ui_listWidget->setSortMode(RamObject::ShortName);
}

RamUser *UserManagerWidget::createObject()
{
    RamUser *user = new RamUser("NEW","J-Doe");
    user->edit();
    return user;
}

