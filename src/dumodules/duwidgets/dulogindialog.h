#ifndef DULOGINDIALOG_H
#define DULOGINDIALOG_H

#include <QDialog>

#include "duwidgets/dulineedit.h"

class DuLoginDialog : public QDialog
{
    Q_OBJECT
public:
    DuLoginDialog(const QString &usernameLabel = "Username", QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    QString username() const { return ui_usernameEdit->text(); }
    QString password() const { return ui_passwordEdit->text(); }

private:
    DuLineEdit *ui_usernameEdit;
    DuLineEdit *ui_passwordEdit;
    QToolButton *ui_showPasswordButton;
    QPushButton *ui_forgotButton;
};

#endif // DULOGINDIALOG_H
