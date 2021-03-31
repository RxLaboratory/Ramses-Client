#ifndef USERSMANAGERWIDGET_H
#define USERSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "usereditwidget.h"

class UsersManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    UsersManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newUser(RamUser *user);
    void userRemoved(RamObject *user);
    void userChanged();
private:
    UserEditWidget *userWidget;
};

#endif // USERSMANAGERWIDGET_H
