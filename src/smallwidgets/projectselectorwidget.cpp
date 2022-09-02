#include "projectselectorwidget.h"

#include "ramses.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent):
    RamObjectListComboBox(
        parent)
{
    m_projectFilter = new RamProjectFilterModel(this);
    this->setModel(m_projectFilter);
    this->setMinimumWidth(200);

    m_pm = ProcessManager::instance();

    connect(this, &RamObjectListComboBox::currentObjectChanged, this,  &ProjectSelectorWidget::setCurrentProject);
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
    connect(Ramses::instance(), &Ramses::userChanged, this, &ProjectSelectorWidget::userChanged);
}

void ProjectSelectorWidget::setCurrentProject(RamObject *projObj)
{
    //if (m_pm->isBusy()) return;
    Ramses::instance()->setCurrentProject( RamProject::c( projObj ) );
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
    if (user) m_projectFilter->addUser(user->uuid());
}
