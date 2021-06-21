#include "ramuserwidget.h"

RamUserWidget::RamUserWidget(RamUser *user, QWidget *parent):
    RamObjectWidget(user, parent)
{
    m_user = user;

    UserEditWidget *uw = new UserEditWidget(user, this);
    setEditWidget(uw);

    setUserEditRole(RamUser::Lead);

    userChanged();
    currentUserChanged(Ramses::instance()->currentUser());

    connect(user, &RamObject::changed, this, &RamUserWidget::userChanged);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &RamUserWidget::currentUserChanged);
}

RamUser *RamUserWidget::user() const
{
    return m_user;
}

void RamUserWidget::exploreClicked()
{
    explore(Ramses::instance()->path(m_user));
}

void RamUserWidget::userChanged()
{
    if (m_user->role() == RamUser::Lead) setIcon(":/icons/lead");
    else if (m_user->role() == RamUser::ProjectAdmin) setIcon(":/icons/project-admin");
    else if (m_user->role() == RamUser::Admin) setIcon(":/icons/admin");
    else setIcon(":/icons/user");
}

void RamUserWidget::currentUserChanged(RamUser *user)
{
     showExploreButton( user->role() == RamUser::Admin || user->is(m_user) );
}
