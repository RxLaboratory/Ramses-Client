#ifndef USEREDITWIDGET_H
#define USEREDITWIDGET_H

#include <QComboBox>
#include <QToolButton>

#include "objecteditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class UserEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit UserEditWidget(RamUser *user, QWidget *parent = nullptr);
    explicit UserEditWidget(QWidget *parent = nullptr);

    RamUser *user() const;

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void changePassword();
    bool checkPasswordInput();

private:
    RamUser *m_user;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *ui_folderWidget;
    QComboBox *ui_roleBox;
    QLineEdit *ui_cpasswordEdit;
    QLineEdit *ui_npassword1Edit;
    QLineEdit *ui_npassword2Edit;
    QLabel *ui_folderLabel;
    QToolButton *ui_passwordButton;
};

#endif // USEREDITWIDGET_H
