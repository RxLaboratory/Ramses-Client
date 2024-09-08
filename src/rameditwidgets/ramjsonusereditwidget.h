#ifndef RAMJSONUSEREDITWIDGET_H
#define RAMJSONUSEREDITWIDGET_H

#include "duwidgets/ducombobox.h"
#include "qpushbutton.h"
#include "ramjsonobjecteditwidget.h"

class RamJsonUserEditWidget : public RamJsonObjectEditWidget
{
    Q_OBJECT
public:
    RamJsonUserEditWidget(bool isSelf, bool isAdmin, bool isTeamProject, const QString &uuid, QWidget *parent = nullptr);

    virtual QJsonObject data() const;
    virtual void setData(const QJsonObject &obj, const QString &uuid);

private:
    void setupUi();
    void connectEvents();

    void updatePassword();
    void updateRole(const QVariant &role);
    void updateEmail();
    void resetPassword();

    DuComboBox *ui_roleBox = nullptr;
    DuLineEdit *ui_emailEdit = nullptr;
    QToolButton *ui_emailButton = nullptr;
    QPushButton *ui_passwordButton = nullptr;
    QPushButton *ui_forgotPasswordButton = nullptr;

    bool _isSelf;
    bool _isAdmin;
    bool _isTeamProject;
    bool _canEditLogin;

    QString _role;
    QString _email;
};

#endif // RAMJSONUSEREDITWIDGET_H
