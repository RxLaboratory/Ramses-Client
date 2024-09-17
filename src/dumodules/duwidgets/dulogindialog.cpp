#include "dulogindialog.h"

#include <QMessageBox>
#include <QToolButton>
#include <QPushButton>
#include <QDialogButtonBox>

#include "duapp/duui.h"
#include "duicon.h"

DuLoginDialog::DuLoginDialog(const QString &usernameLabel, bool showSaveBoxes, bool showResetButton, QWidget *parent, Qt::WindowFlags f):
    QDialog(parent, f)
{
    this->setWindowTitle(tr("Sign in"));

    auto formLayout = DuUI::addFormLayout(this);

    auto usernameLayout = DuUI::createBoxLayout(Qt::Horizontal);

    ui_usernameEdit = new DuLineEdit(this);
    ui_usernameEdit->setPlaceholderText(usernameLabel);
    usernameLayout->addWidget(ui_usernameEdit);

    if (showSaveBoxes) {
        ui_saveUsernameBox = new QCheckBox(tr("Save"));
        ui_saveUsernameBox->setToolTip(tr("Save username"));
        usernameLayout->addWidget(ui_saveUsernameBox);
    }

    formLayout->addRow(usernameLabel, usernameLayout);

    auto passwordLayout = DuUI::createBoxLayout(Qt::Horizontal);

    ui_passwordEdit = new DuLineEdit(this);
    ui_passwordEdit->setPlaceholderText(tr("Password"));
    ui_passwordEdit->setEchoMode(QLineEdit::Password);
    passwordLayout->addWidget(ui_passwordEdit);

    ui_showPasswordButton = new QToolButton(this);
    ui_showPasswordButton->setIconSize(QSize(12,12));
    ui_showPasswordButton->setIcon(DuIcon(":/icons/show"));
    ui_showPasswordButton->setCheckable(true);
    ui_showPasswordButton->setToolTip(tr("Show password"));
    passwordLayout->addWidget(ui_showPasswordButton);

    if (showSaveBoxes) {
        ui_savePasswordBox = new QCheckBox(tr("Save"));
        ui_savePasswordBox->setToolTip(tr("Save password"));
        ui_savePasswordBox->setEnabled(false);
        passwordLayout->addWidget(ui_savePasswordBox);
    }

    formLayout->addRow(tr("Password"), passwordLayout);

    if (showResetButton) {
        ui_forgotButton = new QPushButton(this);
        ui_forgotButton->setProperty("class", "transparent");
        ui_forgotButton->setIconSize(QSize(16,16));
        ui_forgotButton->setIcon(DuIcon(":/icons/forgot-password"));
        ui_forgotButton->setText("Forgot password?");
        formLayout->addRow(ui_forgotButton);
    }

    auto buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    formLayout->addWidget(buttons);
    buttons->button(QDialogButtonBox::Ok)->setText(tr("Sign in"));

    connect(buttons, &QDialogButtonBox::accepted, this, &DuLoginDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &DuLoginDialog::reject);

    connect(ui_showPasswordButton, &QToolButton::clicked, this, [this] (bool c) {
        if (c)
            ui_passwordEdit->setEchoMode(QLineEdit::Normal);
        else
            ui_passwordEdit->setEchoMode(QLineEdit::Password);
    });

    if (ui_forgotButton)
        connect(ui_forgotButton, &QToolButton::clicked, this, [this] () {
            if (ui_usernameEdit->text() == ""){
                QMessageBox::information(
                    this,
                    tr("Invalid username"),
                    tr("Please input your username to reset your password"));
                return;
            }
            if (QMessageBox::question(
                    this,
                    tr("Password reset"),
                    tr("You're about to reset your current password.\n"
                       "Do you want to continue?")
                    ) != QMessageBox::Yes)
                return;
            this->done(-1);
        });

    if (ui_saveUsernameBox)
        connect(ui_saveUsernameBox, &QCheckBox::toggled,
                ui_savePasswordBox, &QCheckBox::setEnabled);
}
