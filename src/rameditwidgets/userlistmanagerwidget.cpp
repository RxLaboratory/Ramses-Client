#include "userlistmanagerwidget.h"

UserListManagerWidget::UserListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->users(),
        new UserEditWidget(),
        "Users",
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void UserListManagerWidget::createObject()
{
    RamUser *user = new RamUser("NEW","J. Doe");

    Ramses::instance()->users()->append(user);

    editObject(user);
}

