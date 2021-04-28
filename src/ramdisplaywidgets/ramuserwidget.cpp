#include "ramuserwidget.h"

#include "mainwindow.h"

RamUserWidget::RamUserWidget(RamUser *user, QWidget *parent):
    RamObjectWidget(user, parent)
{
    m_user = user;

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
    return m_user;
}

void RamUserWidget::userChanged()
{
    if (m_user->role() == RamUser::Lead) setIcon(":/icons/lead");
    else if (m_user->role() == RamUser::ProjectAdmin) setIcon(":/icons/project-admin");
    else if (m_user->role() == RamUser::Admin) setIcon(":/icons/admin");
    else setIcon(":/icons/user");
}
