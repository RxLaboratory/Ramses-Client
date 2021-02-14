#include "userprofilepage.h"

UserProfilePage::UserProfilePage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    profileWidget = new UserEditWidget(this);
    profileWidget->setEnabled(false);
    mainLayout->addWidget(profileWidget);

    connect(Ramses::instance(),&Ramses::loggedIn,this,&UserProfilePage::loggedIn);
    connect(Ramses::instance(),&Ramses::loggedOut,this,&UserProfilePage::loggedOut);
}

void UserProfilePage::loggedIn(RamUser *user)
{
    profileWidget->setUser(user);
    profileWidget->setEnabled(true);
}

void UserProfilePage::loggedOut()
{
    profileWidget->setUser(Ramses::instance()->defaultUser());
    profileWidget->setEnabled(false);
}

