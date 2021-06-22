#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent):
    RamObjectListComboBox(
        Ramses::instance()->projects(),
        parent)
{
    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(setCurrentProject(RamObject*)));
    connect(Ramses::instance(), SIGNAL(currentProjectChanged(RamProject*)), this, SLOT(currentProjectChanged(RamProject*)));
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
