#include "ramuserwidget.h"

#include "mainwindow.h"

RamUserWidget::RamUserWidget(RamUser *user, QWidget *parent):
    RamObjectWidget(user, parent)
{
    _user = user;

    UserEditWidget *uw = new UserEditWidget(user, this);
    setEditWidget(uw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addUserEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Lead);

    userChanged();

    connect(user, &RamObject::changed, this, &RamUserWidget::userChanged);
}

RamUser *RamUserWidget::user() const
{
    return _user;
}

void RamUserWidget::userChanged()
{
    if (_user->role() == RamUser::Lead) setIcon(":/icons/lead");
    else if (_user->role() == RamUser::ProjectAdmin) setIcon(":/icons/project-admin");
    else if (_user->role() == RamUser::Admin) setIcon(":/icons/admin");
    else setIcon(":/icons/user");
}
