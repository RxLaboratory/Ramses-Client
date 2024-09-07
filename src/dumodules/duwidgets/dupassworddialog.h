#ifndef DUPASSWORDDIALOG_H
#define DUPASSWORDDIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

#include "dulineedit.h"

class DuPasswordDialog : public QDialog
{
    Q_OBJECT
public:
    DuPasswordDialog(
        const QString &username,
        bool useCurrent,
        QWidget *parent = nullptr,
        Qt::WindowFlags f = Qt::WindowFlags()
        );

    QString newPassword() const {
        if (isValid()) return ui_new1Edit->text();
        else return "";
    }
    QString currentPassword() const {
        if (ui_currentEdit) return ui_currentEdit->text();
        else return "";
    }

private:
    bool isValid() const;
    void validate();

    DuLineEdit *ui_currentEdit = nullptr;
    DuLineEdit *ui_new1Edit;
    DuLineEdit *ui_new2Edit;
    QToolButton *ui_showPasswordButton;
    QDialogButtonBox *ui_buttons;
};

#endif // DUPASSWORDDIALOG_H
