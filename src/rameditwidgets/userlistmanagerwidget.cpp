#include "userlistmanagerwidget.h"

UserListManagerWidget::UserListManagerWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
    connectEvents();
}

void UserListManagerWidget::editNewUser(RamObject *u)
{
    if (u->shortName() == "NEW")
    {
        m_listEditWidget->select(u);
        m_editWidget->setObject(u);
    }
}
void UserListManagerWidget::setupUi()
{
    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setContentsMargins(3,0,0,0);

    QSplitter *splitter = new QSplitter(this);
    splitter->setOrientation(Qt::Horizontal);
    splitter->setHandleWidth(9);

    m_listEditWidget = new ObjectListEditWidget( Ramses::instance()->users(), false, this);
    splitter->addWidget(m_listEditWidget);

    QWidget *editWidget = new QWidget(this);
    QHBoxLayout *editLayout = new QHBoxLayout(editWidget);

    editLayout->addStretch();

    m_editWidget = new UserEditWidget(this);
    editLayout->addWidget(m_editWidget);

    editLayout->addStretch();

    editLayout->setStretch(0,20);
    editLayout->setStretch(1,80);
    editLayout->setStretch(2,20);

    splitter->addWidget(editWidget);

    lay->addWidget(splitter);
}

void UserListManagerWidget::connectEvents()
{
    connect( m_listEditWidget, &ObjectListEditWidget::objectSelected, m_editWidget, &UserEditWidget::setObject );
    connect( m_listEditWidget, &ObjectListEditWidget::add, Ramses::instance(), &Ramses::createUser );
    connect(Ramses::instance()->users(), &RamObjectList::objectAdded, this, &UserListManagerWidget::editNewUser);
}
