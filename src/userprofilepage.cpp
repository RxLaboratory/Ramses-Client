#include "userprofilepage.h"

UserProfilePage::UserProfilePage(QWidget *parent) :
    QWidget(parent)
{
    setupUi();

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
    profileWidget->setUser(nullptr);
    profileWidget->setEnabled(false);
}

void UserProfilePage::setupUi()
{
    QHBoxLayout *layout = new QHBoxLayout();

    layout->addStretch();

    profileWidget = new UserEditWidget(this);
    profileWidget->setEnabled(false);
    layout->addWidget(profileWidget);

    layout->addStretch();

    layout->setStretch(0,25);
    layout->setStretch(1,50);
    layout->setStretch(2,25);

    this->setLayout(layout);
}

