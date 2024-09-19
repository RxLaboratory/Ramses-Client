#include "ramjsonusereditwidget.h"

#include <QMessageBox>
#include <QToolButton>

#include "duapp/duui.h"
#include "duwidgets/duicon.h"
#include "duwidgets/dupassworddialog.h"
#include "ramserverclient.h"
#include "ramuser.h"

RamJsonUserEditWidget::RamJsonUserEditWidget(bool isSelf, bool isAdmin, bool isTeamProject, const QString &uuid, QWidget *parent):
    RamJsonObjectEditWidget(uuid, parent),
    _isSelf(isSelf),
    _isAdmin(isAdmin),
    _isTeamProject(isTeamProject),
    _canEditLogin((isSelf || isAdmin) && isTeamProject),
    _role(RamUser::ENUMVALUE_Standard)
{
    setupUi();
    connectEvents();
}

QJsonObject RamJsonUserEditWidget::data() const
{
    QJsonObject obj = ramObjectData();

    // Insert role
    if (ui_roleBox)
        obj.insert("role", ui_roleBox->currentData().toString());

    // Insert email
    if (ui_emailEdit)
        obj.insert("email", ui_emailEdit->text());

    return obj;
}

void RamJsonUserEditWidget::setData(const QJsonObject &obj)
{
    setRamObjectData(obj);

    // Set role
    if (ui_roleBox)
        ui_roleBox->setCurrentData(obj.value("role").toString(RamUser::ENUMVALUE_Standard));

    // Get email
    if (ui_emailEdit) {
        if (exists()) {
            QJsonObject rep = RamServerClient::i()->getEmail(
                uuid()
                );
            if (rep.value("success").toBool()) {
                ui_emailEdit->setText(
                    rep.value("content").toString()
                    );
            }
        }
        else {
            ui_emailEdit->setText(obj.value("email").toString());
        }
    }

    emit dataChanged(data());
}

void RamJsonUserEditWidget::setupUi()
{
    // No role and no login info if not a team project
    if (_isTeamProject) {

        // Role

        ui_roleBox = new DuComboBox(this);
        ui_roleBox->addItem(DuIcon(":/icons/admin"), "Administrator", RamUser::ENUMVALUE_Admin);
        ui_roleBox->addItem(DuIcon(":/icons/project-admin"), "Project admin", RamUser::ENUMVALUE_ProjectAdmin);
        ui_roleBox->addItem(DuIcon(":/icons/lead"), "Step lead", RamUser::ENUMVALUE_Lead);
        ui_roleBox->addItem(DuIcon(":/icons/user"), "Standard user", RamUser::ENUMVALUE_Standard);
        ui_propertiesWidget->attributesLayout()->insertRow(0, tr("Role"), ui_roleBox);
        ui_roleBox->setEnabled(!_isSelf && _isAdmin);

        // Login info

        if (_canEditLogin){

            ui_propertiesWidget->mainLayout()->addWidget(
                new QLabel("<b>"+tr("Login information")+"</b>")
                );
            auto loginLayout = DuUI::createFormLayout();
            DuUI::addBlock(loginLayout, ui_propertiesWidget->mainLayout());

            auto emailLayout = DuUI::createBoxLayout(Qt::Horizontal);

            ui_emailEdit = new DuLineEdit(this);
            ui_emailEdit->setPlaceholderText("user@example.com");
            emailLayout->addWidget(ui_emailEdit);

            if (exists()) {
                ui_emailButton = new QToolButton(this);
                ui_emailButton->setIcon(DuIcon(":/icons/check"));
                emailLayout->addWidget(ui_emailButton);
            }

            loginLayout->addRow(tr("E-mail"), emailLayout);

            // Password button
            if (exists()) {
                ui_passwordButton = new QPushButton(this);
                ui_passwordButton->setProperty("class", "transparent");
                ui_passwordButton->setIcon(DuIcon(":/icons/password"));
                ui_passwordButton->setText("Change password...");
                loginLayout->addWidget(ui_passwordButton);

                ui_forgotPasswordButton = new QPushButton(this);
                ui_forgotPasswordButton->setProperty("class", "transparent");
                ui_forgotPasswordButton->setIcon(DuIcon(":/icons/forgot-password"));
                ui_forgotPasswordButton->setText("Reset password...");
                loginLayout->addWidget(ui_forgotPasswordButton);
            }
        }
    }

    ui_propertiesWidget->mainLayout()->addStretch(1);
}

void RamJsonUserEditWidget::connectEvents()
{
    if (ui_roleBox)
        connect(ui_roleBox, &DuComboBox::dataActivated, this, &RamJsonUserEditWidget::updateRole);
    if (ui_passwordButton)
        connect(ui_passwordButton, &QToolButton::clicked, this, &RamJsonUserEditWidget::updatePassword);
    if (ui_forgotPasswordButton)
        connect(ui_forgotPasswordButton, &QToolButton::clicked, this, &RamJsonUserEditWidget::resetPassword);
    if (ui_emailButton)
        connect(ui_emailButton, &QToolButton::clicked, this, &RamJsonUserEditWidget::updateEmail);
    else if (ui_emailEdit)
        connect(ui_emailEdit, &DuLineEdit::editingFinished, this, &RamJsonUserEditWidget::emitEdited);
}

void RamJsonUserEditWidget::updatePassword()
{
    QJsonObject d = data();

    DuPasswordDialog dialog(d.value("name").toString(), _isSelf, this );
    if (DuUI::execDialog(&dialog) != QDialog::Accepted)
        return;

    QJsonObject rep = RamServerClient::i()->setPassword(
        d.value("uuid").toString(),
        dialog.newPassword(),
        dialog.currentPassword()
        );

    if (!rep.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Failed to set password"),
                             rep.value("message").toString("Unknown error")
                             );
    }
    else {
        QMessageBox::information(this,
                                 tr("Password changed"),
                                 tr("Password successfully changed!")
                                 );
    }
}

void RamJsonUserEditWidget::updateRole(const QVariant &role)
{

    QString r = role.toString();

    // If it exists, update on server
    if (exists()) {
        QJsonObject rep = RamServerClient::i()->setUserRole(
            uuid(),
            r
            );

        if (!rep.value("success").toBool(false)) {
            QMessageBox::warning(this,
                                 tr("Failed to set role"),
                                 rep.value("message").toString("Unknown error")
                                 );
            // Revert
            ui_roleBox->setCurrentData(_role);
            return;
        }
        else {
            QMessageBox::information(this,
                                     tr("Role changed"),
                                     tr("Role changed!")
                                     );
        }
    }

    _role = r;
    emitEdited();
}

void RamJsonUserEditWidget::updateEmail()
{
    QString email = ui_emailEdit->text();
    if (email == "") {
        ui_emailEdit->setText(_email);
        return;
    }

    // If it exists, update on server
    if (exists()) {
        QJsonObject rep = RamServerClient::i()->setEmail(
            uuid(),
            email
            );

        if (!rep.value("success").toBool(false)) {
            QMessageBox::warning(this,
                                 tr("Failed to set email"),
                                 rep.value("message").toString("Unknown error")
                                 );
            // Revert
            ui_emailEdit->setText(_email);
            return;
        }
        else {
            QMessageBox::information(this,
                                     tr("E-mail changed"),
                                     tr("E-mail successfully changed!")
                                     );
        }
    }

    _email = email;
    emitEdited();
}

void RamJsonUserEditWidget::resetPassword()
{
    if (QMessageBox::question(
            this,
            tr("Password reset"),
            tr("A new password will be sent to your email address.\n"
               "Do you want to continue?")
            ) != QMessageBox::Yes)
        return;

    QJsonObject d = data();

    QJsonObject rep = RamServerClient::i()->resetPassword(
        d.value("uuid").toString()
        );

    if (!rep.value("success").toBool(false)) {
        QMessageBox::warning(this,
                             tr("Failed to reset password"),
                             rep.value("message").toString("Unknown error")
                             );
    }
    else {
        QMessageBox::information(this,
                                 tr("Password changed"),
                                 rep.value("message").toString("Password successfully changed!")
                                 );
    }
}
