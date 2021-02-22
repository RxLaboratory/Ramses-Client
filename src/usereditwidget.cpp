#include "usereditwidget.h"

UserEditWidget::UserEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _user = nullptr;

    folderSelector = new DuQFFolderSelectorWidget(this);
    folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    folderLayout->addWidget(folderSelector);

    roleBox->setCurrentIndex(0);

    connect(profileUpdateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(cpasswordEdit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(npassword1Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(npassword2Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &UserEditWidget::updateFolderLabel);
    connect(DBInterface::instance(),&DBInterface::log, this, &UserEditWidget::dbiLog);


    this->setEnabled(false);
}

RamUser *UserEditWidget::user() const
{
    return _user;
}

void UserEditWidget::setUser(RamUser *user)
{
    disconnect(_currentUserConnection);

    nameEdit->setText("");
    shortNameEdit->setText("");
    cpasswordEdit->setText("");
    npassword1Edit->setText("");
    npassword2Edit->setText("");
    roleBox->setCurrentIndex(0);
    roleBox->setEnabled(true);
    roleBox->setToolTip("");
    cpasswordEdit->setEnabled(false);
    folderSelector->setPath("");
    folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    updateFolderLabel("");
    this->setEnabled(false);

    _user = user;
    RamUser *current = Ramses::instance()->currentUser();
    if (!user || !current) return;

    nameEdit->setText(user->name());
    shortNameEdit->setText(user->shortName());

    roleBox->setCurrentIndex(user->role());

    if (user->folderPath() != "auto") folderSelector->setPath( user->folderPath() );
    folderSelector->setPlaceHolderText( Ramses::instance()->defaultUserPath(user) );
    folderLabel->setText( Ramses::instance()->userPath(user) );

    if (user->uuid() == current->uuid())
    {
        roleBox->setEnabled(false);
        roleBox->setToolTip("You cannot change your own role!");
        cpasswordEdit->setEnabled(true);
        this->setEnabled(true);
    }
    else
    {
        this->setEnabled(Ramses::instance()->isAdmin());
    }

    _currentUserConnection = connect(user,&RamUser::destroyed, this, &UserEditWidget::userDestroyed);

}

void UserEditWidget::update()
{
    if (!_user) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _user->setName( nameEdit->text() );
    _user->setShortName( shortNameEdit->text() );
    _user->setFolderPath( folderSelector->path());

    int roleIndex = roleBox->currentIndex();
    if (roleIndex == 3) _user->setRole(RamUser::Admin);
    else if (roleIndex == 2) _user->setRole(RamUser::ProjectAdmin);
    else if (roleIndex == 1) _user->setRole(RamUser::Lead);
    else _user->setRole(RamUser::Standard);

    _user->update();

    if (npassword1Edit->text() != "")
    {
        _user->updatePassword(
                    cpasswordEdit->text(),
                    npassword1Edit->text() );
    }

    npassword1Edit->setText("");
    npassword2Edit->setText("");
    cpasswordEdit->setText("");

    this->setEnabled(true);
}

void UserEditWidget::revert()
{
    setUser(_user);
}

bool UserEditWidget::checkInput()
{
    if (!_user) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("You must choose a user name!");
        profileUpdateButton->setEnabled(false);
        return false;
    }

    if (npassword1Edit->text() != "")
    {
        if (cpasswordEdit->text() == "" && _user->uuid() == Ramses::instance()->currentUser()->uuid())
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

void UserEditWidget::updateFolderLabel(QString path)
{
    if (path != "") folderLabel->setText( Ramses::instance()->pathFromMain(path) );
    else if (_user) folderLabel->setText( Ramses::instance()->userPath(_user) );
}

void UserEditWidget::userDestroyed(QObject */*o*/)
{
    setUser(nullptr);
}

void UserEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
