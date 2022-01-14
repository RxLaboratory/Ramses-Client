#include "userlistmanagerwidget.h"

UserListManagerWidget::UserListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->users(),
        "Users",
        QIcon(":icons/user"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "Ramses" << "Removed";
    m_listEditWidget->setDontRemoveShortNameList(dontRemove);
}

RamObject *UserListManagerWidget::createObject()
{
    RamUser *user = new RamUser("NEW","J-Doe");

    Ramses::instance()->users()->append(user);
    user->edit();
    return user;
}

