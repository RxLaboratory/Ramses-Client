#ifndef DULOGINDIALOG_H
#define DULOGINDIALOG_H

#include <QDialog>
#include <QCheckBox>

#include "duwidgets/dulineedit.h"

class DuLoginDialog : public QDialog
{
    Q_OBJECT
public:
    DuLoginDialog(const QString &usernameLabel = "Username", bool showSaveBoxes = true, bool showResetButton = true, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    QString username() const { return ui_usernameEdit->text(); }
    QString password() const { return ui_passwordEdit->text(); }
    bool saveUsername() const {
        if (ui_saveUsernameBox) return ui_saveUsernameBox->isChecked();
        return false;
    }
    bool savePassword() const {
        if (ui_savePasswordBox && ui_savePasswordBox->isEnabled())
            return ui_savePasswordBox->isChecked();
        return false;
    }

    void setUsername(const QString &username) {
        ui_saveUsernameBox->setChecked(username != "");
        ui_usernameEdit->setText(username);
    }

private:
    DuLineEdit *ui_usernameEdit;
    DuLineEdit *ui_passwordEdit;
    QToolButton *ui_showPasswordButton;
    QPushButton *ui_forgotButton = nullptr;
    QCheckBox *ui_saveUsernameBox = nullptr;
    QCheckBox *ui_savePasswordBox = nullptr;
};

#endif // DULOGINDIALOG_H
