#include "projectselectorwidget.h"

#include "ramses.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent):
    RamObjectComboBox( parent )
{
    m_projectFilter = new RamObjectSortFilterProxyModel(this);
    m_projectFilter->setSourceModel(Ramses::i()->projects());
    this->setModel(m_projectFilter);
    this->setMinimumWidth(200);

    m_pm = ProgressManager::instance();

    connect(this, &RamObjectComboBox::currentObjectChanged, this,  &ProjectSelectorWidget::setCurrentProject);
    connect(Ramses::i(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
    connect(Ramses::i(), &Ramses::ready, this, &ProjectSelectorWidget::ramsesReady);
}

void ProjectSelectorWidget::setCurrentProject(RamObject *projObj)
{
    //if (m_pm->isBusy()) return;
    Ramses::i()->setCurrentProject( RamProject::c( projObj ) );
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

void ProjectSelectorWidget::ramsesReady()
{
    m_projectFilter->clearFilterListUuids();
    m_projectFilter->addFilterUuid(
        Ramses::i()->currentUser()->uuid()
        );
}
