#include "usersmanagerwidget.h"

UsersManagerWidget::UsersManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    userWidget = new UserEditWidget(this);
    this->setWidget(userWidget);
    userWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    foreach(RamUser *user, Ramses::instance()->users()) newUser(user);

    connect(Ramses::instance(), &Ramses::newUser, this, &UsersManagerWidget::newUser);
}

void UsersManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamUser *user, Ramses::instance()->users())
    {
        if (user->uuid() == data.toString())
        {
            userWidget->setUser(user);
            userWidget->setEnabled(true);
            return;
        }
    }
    userWidget->setEnabled(false);
}

void UsersManagerWidget::createItem()
{
    Ramses::instance()->createUser();
}

void UsersManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeUser(data.toString());
}

void UsersManagerWidget::newUser(RamUser *user)
{
    if (!user) return;
    if (user->uuid() != "")
    {
        QListWidgetItem *userItem = new QListWidgetItem(user->name());
        userItem->setData(Qt::UserRole, user->uuid());
        this->addItem(userItem);
        connect(user,&RamUser::removed,this,&UsersManagerWidget::userRemoved);
        connect(user,&RamUser::changed, this, &UsersManagerWidget::userChanged);
    }
}

void UsersManagerWidget::userRemoved(RamObject *user)
{
    removeData(user->uuid());
}

void UsersManagerWidget::userChanged()
{
    RamUser *user = (RamUser*)QObject::sender();
    updateItem(user->uuid(), user->name());
}

