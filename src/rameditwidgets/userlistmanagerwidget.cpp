#include "userlistmanagerwidget.h"

UserListManagerWidget::UserListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->users(),
        new UserEditWidget(),
        "Users",
        parent )
{

}

void UserListManagerWidget::createObject()
{
    Ramses::instance()->createUser();
}

