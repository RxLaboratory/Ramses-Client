#include "userprofilepage.h"

UserProfilePage::UserProfilePage(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    profileWidget->setEnabled(false);

    _ramses = Ramses::instance();

    connect(_ramses,&Ramses::loggedIn,this,&UserProfilePage::loggedIn);
    connect(_ramses,&Ramses::loggedOut,this,&UserProfilePage::loggedOut);
    connect(DBInterface::instance(), &DBInterface::log, this, &UserProfilePage::dbiLog);
    connect(profileUpdateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &UserProfilePage::checkInput);
    connect(cpasswordEdit, &QLineEdit::textChanged, this, &UserProfilePage::checkInput);
    connect(npassword1Edit, &QLineEdit::textChanged, this, &UserProfilePage::checkInput);
    connect(npassword2Edit, &QLineEdit::textChanged, this, &UserProfilePage::checkInput);
}

void UserProfilePage::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}

void UserProfilePage::loggedIn(RamUser *user)
{
    nameEdit->setText(user->name());
    shortNameEdit->setText(user->shortName());
    if (user->role() == RamUser::Admin) roleLabel->setText("Administrator");
    else if (user->role() == RamUser::Lead) roleLabel->setText("Lead");
    else roleLabel->setText("Standard User");

    profileWidget->setEnabled(true);
}

void UserProfilePage::loggedOut()
{
    nameEdit->setText("");
    shortNameEdit->setText("");
    roleLabel->setText("Standard User");

    profileWidget->setEnabled(false);
}

void UserProfilePage::update()
{
    profileWidget->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        profileWidget->setEnabled(true);
        return;
    }

    _ramses->currentUser()->setName( nameEdit->text() );
    _ramses->currentUser()->setShortName( shortNameEdit->text() );
    _ramses->currentUser()->update();

    if (npassword1Edit->text() != "")
    {
        _ramses->currentUser()->updatePassword(
                    cpasswordEdit->text(),
                    npassword1Edit->text() );
    }

    npassword1Edit->setText("");
    npassword2Edit->setText("");
    cpasswordEdit->setText("");

    statusLabel->setText("Updating profile...");
    profileWidget->setEnabled(true);
}

bool UserProfilePage::checkInput()
{
    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("You must choose a user name!");
        profileUpdateButton->setEnabled(false);
        return false;
    }

    if (npassword1Edit->text() != "")
    {
        if (cpasswordEdit->text() == "")
        {
            statusLabel->setText("You must specify your current password to be able to modify it.");
            profileUpdateButton->setEnabled(false);
            return false;
        }
        if (npassword1Edit->text() != npassword2Edit->text())
        {
            statusLabel->setText("The two fields for the new password are different.");
            profileUpdateButton->setEnabled(false);
            return false;
        }
    }

    statusLabel->setText("");
    profileUpdateButton->setEnabled(true);
    return true;
}
