#include "usereditwidget.h"

UserEditWidget::UserEditWidget(RamUser *user, QWidget *parent) :
    ObjectEditWidget(user, parent)
{
    setupUi();
    connectEvents();

    setObject(user);
}

UserEditWidget::UserEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);
}

RamUser *UserEditWidget::user() const
{
    return _user;
}

void UserEditWidget::setObject(RamObject *obj)
{
    RamUser *user = (RamUser*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(user);
    _user = user;

    QSignalBlocker b1(cpasswordEdit);
    QSignalBlocker b2(npassword1Edit);
    QSignalBlocker b3(npassword2Edit);
    QSignalBlocker b4(roleBox);
    QSignalBlocker b5(folderSelector);

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

    RamUser *current = Ramses::instance()->currentUser();
    if (!user || !current) return;

    roleBox->setCurrentIndex(user->role());

    if (user->folderPath() != "auto") folderSelector->setPath( user->folderPath() );
    folderSelector->setPlaceHolderText( Ramses::instance()->defaultUserPath(user) );
    folderLabel->setText( Ramses::instance()->path(user) );

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

    _objectConnections << connect(user, &RamUser::changed, this, &UserEditWidget::userChanged);
}

void UserEditWidget::userChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_user);
}

void UserEditWidget::changePassword()
{
    if (!checkInput()) return;

    if (npassword1Edit->text() != "")
    {
        _user->updatePassword(
                    cpasswordEdit->text(),
                    npassword1Edit->text() );
    }

    npassword1Edit->setText("");
    npassword2Edit->setText("");
    cpasswordEdit->setText("");
}

void UserEditWidget::update()
{
    if (!_user) return;

    updating = true;

    _user->setFolderPath( folderSelector->path());

    int roleIndex = roleBox->currentIndex();
    if (roleIndex == 3) _user->setRole(RamUser::Admin);
    else if (roleIndex == 2) _user->setRole(RamUser::ProjectAdmin);
    else if (roleIndex == 1) _user->setRole(RamUser::Lead);
    else _user->setRole(RamUser::Standard);

    ObjectEditWidget::update();

    updating = false;
}

bool UserEditWidget::checkInput()
{
    if (!_user) return false;

    return ObjectEditWidget::checkInput();

    if (npassword1Edit->text() != "")
    {
        if (cpasswordEdit->text() == "" && _user->uuid() == Ramses::instance()->currentUser()->uuid())
        {
            statusLabel->setText("You must specify your current password to be able to modify it.");
            return false;
        }
        if (npassword1Edit->text() != npassword2Edit->text())
        {
            statusLabel->setText("The two fields for the new password are different.");
            return false;
        }
    }

    statusLabel->setText("");

    return true;
}

void UserEditWidget::updateFolderLabel(QString path)
{
    if (path != "") folderLabel->setText( Ramses::instance()->pathFromRamses(path) );
    else if (_user) folderLabel->setText( Ramses::instance()->path(_user) );
}

void UserEditWidget::setupUi()
{
    QLabel *roleLabel = new QLabel("Current role", this);
    mainFormLayout->addWidget(roleLabel, 2, 0);

    roleBox = new QComboBox(this);
    roleBox->addItem(QIcon(":/icons/user"), "Standard");
    roleBox->addItem(QIcon(":/icons/lead"), "Lead");
    roleBox->addItem(QIcon(":/icons/project-admin"), "Project Admin");
    roleBox->addItem(QIcon(":/icons/admin"), "Administrator");
    roleBox->setCurrentIndex(0);
    mainFormLayout->addWidget(roleBox, 2, 1);

    QLabel *currentPasswordLabel = new QLabel("Current password", this);
    mainFormLayout->addWidget(currentPasswordLabel, 3, 0);

    cpasswordEdit = new QLineEdit(this);
    cpasswordEdit->setEchoMode(QLineEdit::Password);
    mainFormLayout->addWidget(cpasswordEdit, 3, 1);

    QLabel *newPasswordLabel = new QLabel("New password", this);
    mainFormLayout->addWidget(newPasswordLabel, 4, 0);

    npassword1Edit = new QLineEdit(this);
    npassword1Edit->setEchoMode(QLineEdit::Password);
    mainFormLayout->addWidget(npassword1Edit, 4, 1);

    npassword2Edit = new QLineEdit(this);
    npassword2Edit->setEchoMode(QLineEdit::Password);
    mainFormLayout->addWidget(npassword2Edit, 5, 1);

    passwordButton = new QToolButton(this);
    passwordButton->setText("Change password");
    mainFormLayout->addWidget(passwordButton, 6, 1);

    QLabel *uFolderLabel = new QLabel("Personal folder", this);
    mainFormLayout->addWidget(uFolderLabel, 7, 0);

    folderSelector = new DuQFFolderSelectorWidget(DuQFFolderSelectorWidget::Folder, this);
    folderSelector->setPlaceHolderText("Default (Ramses/Users/User_ShortName)");
    mainFormLayout->addWidget(folderSelector, 7, 1);

    folderLabel = new QLabel(this);
    folderLabel->setEnabled(false);
    mainLayout->insertWidget(1, folderLabel);

    mainLayout->addStretch();
}

void UserEditWidget::connectEvents()
{
    connect(cpasswordEdit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(npassword1Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(npassword2Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(roleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(passwordButton, SIGNAL(clicked()), this, SLOT(changePassword()));
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanging, this, &UserEditWidget::updateFolderLabel);
    connect(folderSelector, &DuQFFolderSelectorWidget::pathChanged, this, &UserEditWidget::update);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &UserEditWidget::userChanged);
}
