#include "dulogindialog.h"

#include <QMessageBox>

#include "duapp/duui.h"

DuLoginDialog::DuLoginDialog(const QString &usernameLabel, bool showSaveBoxes, QWidget *parent, Qt::WindowFlags f):
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

    ui_forgotButton = new QPushButton(this);
    ui_forgotButton->setProperty("class", "transparent");
    ui_forgotButton->setIconSize(QSize(16,16));
    ui_forgotButton->setIcon(DuIcon(":/icons/forgot-password"));
    ui_forgotButton->setText("Forgot password?");
    formLayout->addRow(ui_forgotButton);

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

    connect(ui_forgotButton, &QToolButton::clicked, this, [this] () {
        QMessageBox::information(this,
                                 tr("Not implemented"),
                                 tr("You can't reset your password yet.\nPlease contact one of your Ramses administrator to reset your password.")
                                 );
    });

    connect(ui_saveUsernameBox, &QCheckBox::toggled,
            ui_savePasswordBox, &QCheckBox::setEnabled);
}
