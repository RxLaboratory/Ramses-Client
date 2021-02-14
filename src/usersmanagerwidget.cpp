#include "usersmanagerwidget.h"

UsersManagerWidget::UsersManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    userWidget = new UserEditWidget(this);
    this->setWidget(userWidget);
    userWidget->setEnabled(false);

    foreach(RamUser *user, Ramses::instance()->users()) newUser(user);

    connect(Ramses::instance(), &Ramses::newUser, this, &UsersManagerWidget::newUser);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &UsersManagerWidget::loggedIn);
    connect(Ramses::instance(), &Ramses::loggedOut, this, &UsersManagerWidget::loggedOut);
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
    if (user->uuid() != "")
    {
        QListWidgetItem *userItem = new QListWidgetItem(user->name());
        userItem->setData(Qt::UserRole, user->uuid());
        this->addItem(userItem);
        connect(user,&RamUser::destroyed,this,&UsersManagerWidget::removeUser);
        connect(user,&RamUser::changed, this, &UsersManagerWidget::userChanged);
    }
}

void UsersManagerWidget::removeUser(QObject *user)
{
    RamUser *u = (RamUser*)user;

    removeData(u->uuid());
}

void UsersManagerWidget::loggedIn(RamUser *user)
{
    this->setEnabled(user->role() == RamUser::Admin);
}

void UsersManagerWidget::loggedOut()
{
    this->setEnabled(false);
}

void UsersManagerWidget::userChanged()
{
    RamUser *user = (RamUser*)QObject::sender();
    updateItem(user->uuid(), user->name());
}

