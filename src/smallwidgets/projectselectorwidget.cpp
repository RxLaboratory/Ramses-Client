#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent):
    RamObjectListComboBox(
        parent)
{
    m_projectFilter = new RamProjectFilterModel(this);
    this->setModel(m_projectFilter);
    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setCurrentProject(RamObject*)));
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
    connect(Ramses::instance(), SIGNAL(loggedIn(RamUser*)), this, SLOT(userChanged(RamUser*)));
}

void ProjectSelectorWidget::setCurrentProject(RamObject *projObj)
{
    RamProject *proj = qobject_cast<RamProject*>( projObj );
    Ramses::instance()->setCurrentProject( proj );
}

void ProjectSelectorWidget::currentProjectChanged(RamProject *p)
{
    QSignalBlocker b(this);

    if (!p)
    {
        setCurrentIndex(-1);
        return;
    }

    setObject(p);
}

void ProjectSelectorWidget::userChanged(RamUser *user)
{
    m_projectFilter->clearUsers();
    m_projectFilter->addUser(user->uuid());
}
