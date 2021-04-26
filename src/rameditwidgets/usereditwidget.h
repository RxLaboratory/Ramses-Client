#ifndef USEREDITWIDGET_H
#define USEREDITWIDGET_H

#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>

#include "objecteditwidget.h"
#include "ramses.h"
#include "duqf-widgets/duqffolderselectorwidget.h"

class UserEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit UserEditWidget(RamUser *user, QWidget *parent = nullptr);
    explicit UserEditWidget(QWidget *parent = nullptr);

    RamUser *user() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void userChanged(RamObject *o);
    void changePassword();
    bool checkInput() Q_DECL_OVERRIDE;
    void updateFolderLabel(QString path);

private:
    RamUser *_user;

    void setupUi();
    void connectEvents();

    DuQFFolderSelectorWidget *folderSelector;
    QComboBox *roleBox;
    QLineEdit *cpasswordEdit;
    QLineEdit *npassword1Edit;
    QLineEdit *npassword2Edit;
    QLabel *folderLabel;
    QToolButton *passwordButton;

    QMetaObject::Connection _currentUserConnection;
};

#endif // USEREDITWIDGET_H
