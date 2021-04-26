#include "ramuserwidget.h"

#include "mainwindow.h"

RamUserWidget::RamUserWidget(RamUser *user, QWidget *parent):
    RamObjectWidget(user, parent)
{
    _user = user;

    UserEditWidget *uw = new UserEditWidget(user, this);
    setEditWidget(uw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    this->dockEditWidget()->setIcon(":/icons/asset");
    mw->addUserEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Lead);

    setIcon(":/icons/asset");
}

RamUser *RamUserWidget::user() const
{
    return _user;
}
