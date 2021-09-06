#include "usereditwidget.h"

UserEditWidget::UserEditWidget(RamUser *user, QWidget *parent) :
    ObjectEditWidget(user, parent)
{
    setupUi();
    connectEvents();

    setObject(user);

     m_dontRename << "Ramses" << "Removed" << "Duduf";
}

UserEditWidget::UserEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();

    setObject(nullptr);

    m_dontRename << "Ramses" << "Removed" << "Duduf";
}

RamUser *UserEditWidget::user() const
{
    return m_user;
}

void UserEditWidget::setObject(RamObject *obj)
{
    RamUser *user = qobject_cast<RamUser*>(obj);

    this->setEnabled(false);

    ObjectEditWidget::setObject(user);
    m_user = user;

    QSignalBlocker b1(ui_cpasswordEdit);
    QSignalBlocker b2(ui_npassword1Edit);
    QSignalBlocker b3(ui_npassword2Edit);
    QSignalBlocker b4(ui_roleBox);
    QSignalBlocker b5(ui_folderWidget);

    ui_cpasswordEdit->setText("");
    ui_npassword1Edit->setText("");
    ui_npassword2Edit->setText("");
    ui_roleBox->setCurrentIndex(0);
    ui_roleBox->setEnabled(true);
    ui_roleBox->setToolTip("");
    ui_cpasswordEdit->setEnabled(false);
    ui_folderWidget->setPath("");

    RamUser *current = Ramses::instance()->currentUser();
    if (!user || !current) return;

    ui_folderWidget->setPath( user->path() );
    ui_roleBox->setCurrentIndex(user->role());

    if (m_dontRename.contains(user->shortName()))
    {
        ui_npassword1Edit->setEnabled(false);
        ui_npassword2Edit->setEnabled(false);
        ui_cpasswordEdit->setEnabled(false);
    }
    else
    {
        ui_npassword1Edit->setEnabled(true);
        ui_npassword2Edit->setEnabled(true);
        if (user->uuid() == current->uuid()) ui_cpasswordEdit->setEnabled(true);
    }

    if (user->uuid() == current->uuid())
    {
        ui_roleBox->setEnabled(false);
        ui_roleBox->setToolTip("You cannot change your own role!");
        ui_cpasswordEdit->setEnabled(true);
        this->setEnabled(true);
    }
    else
    {
        this->setEnabled(Ramses::instance()->isAdmin());
    }
}

void UserEditWidget::changePassword()
{
    if (!checkPasswordInput()) return;

    if (ui_npassword1Edit->text() != "")
    {
        m_user->updatePassword(
                    ui_cpasswordEdit->text(),
                    ui_npassword1Edit->text() );
    }

    ui_npassword1Edit->setText("");
    ui_npassword2Edit->setText("");
    ui_cpasswordEdit->setText("");
}

void UserEditWidget::update()
{
    if (!m_user) return;

    updating = true;

    int roleIndex = ui_roleBox->currentIndex();
    if (roleIndex == 3) m_user->setRole(RamUser::Admin);
    else if (roleIndex == 2) m_user->setRole(RamUser::ProjectAdmin);
    else if (roleIndex == 1) m_user->setRole(RamUser::Lead);
    else m_user->setRole(RamUser::Standard);

    ObjectEditWidget::update();

    updating = false;
}

bool UserEditWidget::checkPasswordInput()
{
    if (!m_user) return false;

    bool ok = ObjectEditWidget::checkInput();
    if (!ok) return false;

    if (ui_npassword1Edit->text() != "")
    {
        if (ui_cpasswordEdit->text() == "" && m_user->is(Ramses::instance()->currentUser()) )
        {
            QMessageBox::warning(this, "What's your password?", "I'm sorry, you have to know your current password to change it.\nPlease try again." );
            return false;
        }
        if (ui_npassword1Edit->text() != ui_npassword2Edit->text())
        {
            QMessageBox::warning(this, "Password mismatch", "I'm sorry, the two fields for the new password are different.\nPlease try again." );
            ui_npassword1Edit->setText("");
            ui_npassword2Edit->setText("");
            return false;
        }
    }

    return true;
}

void UserEditWidget::setupUi()
{
    QLabel *roleLabel = new QLabel("Current role", this);
    ui_mainFormLayout->addWidget(roleLabel, 3, 0);

    ui_roleBox = new QComboBox(this);
    ui_roleBox->addItem(QIcon(":/icons/user"), "Standard");
    ui_roleBox->addItem(QIcon(":/icons/lead"), "Lead");
    ui_roleBox->addItem(QIcon(":/icons/project-admin"), "Project Admin");
    ui_roleBox->addItem(QIcon(":/icons/admin"), "Administrator");
    ui_roleBox->setCurrentIndex(0);
    ui_mainFormLayout->addWidget(ui_roleBox, 3, 1);

    QLabel *currentPasswordLabel = new QLabel("Current password", this);
    ui_mainFormLayout->addWidget(currentPasswordLabel, 4, 0);

    ui_cpasswordEdit = new QLineEdit(this);
    ui_cpasswordEdit->setEchoMode(QLineEdit::Password);
    ui_mainFormLayout->addWidget(ui_cpasswordEdit, 4, 1);

    QLabel *newPasswordLabel = new QLabel("New password", this);
    ui_mainFormLayout->addWidget(newPasswordLabel, 5, 0);

    ui_npassword1Edit = new QLineEdit(this);
    ui_npassword1Edit->setEchoMode(QLineEdit::Password);
    ui_mainFormLayout->addWidget(ui_npassword1Edit, 5, 1);

    QLabel *newPasswordLabel2 = new QLabel("Repeat new password", this);
    ui_mainFormLayout->addWidget(newPasswordLabel2, 6, 0);

    ui_npassword2Edit = new QLineEdit(this);
    ui_npassword2Edit->setEchoMode(QLineEdit::Password);
    ui_mainFormLayout->addWidget(ui_npassword2Edit, 6, 1);

    ui_passwordButton = new QToolButton(this);
    ui_passwordButton->setText("Change password");
    ui_mainFormLayout->addWidget(ui_passwordButton, 7, 1);

    QLabel *uFolderLabel = new QLabel("Personal folder", this);
    ui_mainFormLayout->addWidget(uFolderLabel, 8, 0);

    ui_folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainFormLayout->addWidget(ui_folderWidget, 8, 1);

    ui_mainLayout->addStretch();
}

void UserEditWidget::connectEvents()
{
    connect(ui_cpasswordEdit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(ui_npassword1Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(ui_npassword2Edit, &QLineEdit::textChanged, this, &UserEditWidget::checkInput);
    connect(ui_roleBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
    connect(ui_passwordButton, SIGNAL(clicked()), this, SLOT(changePassword()));
    connect(Ramses::instance(), &Ramses::loggedIn, this, &UserEditWidget::objectChanged);

    monitorDbQuery("updateUser");
    monitorDbQuery("updatePassword");
}
