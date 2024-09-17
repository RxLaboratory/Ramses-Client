#include "dupassworddialog.h"

#include <QLabel>
#include <QToolButton>
#include <QPushButton>

#include "duwidgets/duicon.h"
#include "duapp/duui.h"

DuPasswordDialog::DuPasswordDialog(const QString &username,
                                   bool useCurrent,
                                   QWidget *parent,
                                   Qt::WindowFlags f):
    QDialog(parent, f)
{
    auto mainLayout = DuUI::addBoxLayout(Qt::Vertical, this);
    auto topLayout = DuUI::addBoxLayout(Qt::Horizontal, mainLayout);

    topLayout->addWidget(new QLabel(tr(
                                      "Change password for %1." /// TRANSLATORS: the argument is a username
                                        ).arg(username)
                                    ));
    topLayout->setStretch(0,1);

    ui_showPasswordButton = new QToolButton(this);
    ui_showPasswordButton->setIconSize(QSize(12,12));
    ui_showPasswordButton->setIcon(DuIcon(":/icons/show"));
    ui_showPasswordButton->setCheckable(true);
    ui_showPasswordButton->setToolTip(tr("Show password"));
    topLayout->addWidget(ui_showPasswordButton);
    topLayout->setStretch(1,0);

    auto formLayout = DuUI::addFormLayout(mainLayout);

    if (useCurrent) {
        ui_currentEdit = new DuLineEdit(this);
        ui_currentEdit->setPlaceholderText(tr("Current password"));
        ui_currentEdit->setEchoMode(QLineEdit::Password);
        formLayout->addRow(tr("Current"), ui_currentEdit);
    }

    ui_new1Edit = new DuLineEdit(this);
    ui_new1Edit->setPlaceholderText(tr("New password"));
    ui_new1Edit->setEchoMode(QLineEdit::Password);
    formLayout->addRow(tr("New"), ui_new1Edit);

    ui_new2Edit = new DuLineEdit(this);
    ui_new2Edit->setPlaceholderText(tr("Repeat password"));
    ui_new2Edit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("", ui_new2Edit);

    ui_buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, this);
    mainLayout->addWidget(ui_buttons);

    connect(ui_buttons, &QDialogButtonBox::accepted, this, &DuPasswordDialog::accept);
    connect(ui_buttons, &QDialogButtonBox::rejected, this, &DuPasswordDialog::reject);

    connect(ui_showPasswordButton, &QToolButton::clicked, this, [this] (bool c) {
        if (c) {
            if (ui_currentEdit)
                ui_currentEdit->setEchoMode(QLineEdit::Normal);
            ui_new1Edit->setEchoMode(QLineEdit::Normal);
            ui_new2Edit->setEchoMode(QLineEdit::Normal);
        }
        else {
            if (ui_currentEdit)
                ui_currentEdit->setEchoMode(QLineEdit::Password);
            ui_new1Edit->setEchoMode(QLineEdit::Password);
            ui_new2Edit->setEchoMode(QLineEdit::Password);
        }
    });

    connect(ui_new1Edit, &DuLineEdit::textEdited, this, &DuPasswordDialog::validate);
    connect(ui_new2Edit, &DuLineEdit::textEdited, this, &DuPasswordDialog::validate);

    validate();
}

bool DuPasswordDialog::isValid() const
{
    QString p1 = ui_new1Edit->text();
    QString p2 = ui_new2Edit->text();
    return p1 == p2 && p1 != "";
}

void DuPasswordDialog::validate()
{
    ui_buttons->button(QDialogButtonBox::Save)->setEnabled(
        isValid()
        );
}
