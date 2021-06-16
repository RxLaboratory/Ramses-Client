#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent):
    RamObjectListComboBox(
        Ramses::instance()->projects(),
        parent)
{
    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentProject(int)));
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ProjectSelectorWidget::currentProjectChanged);
}

void ProjectSelectorWidget::setCurrentProject(int index)
{
    Ramses::instance()->setCurrentProject( this->itemData(index).toString() );
}

void ProjectSelectorWidget::currentProjectChanged(RamProject *p)
{
    QSignalBlocker b(this);

    if (!p)
    {
        setCurrentIndex(-1);
        return;
    }

    for (int i = this->count() - 1; i >= 0; i--)
    {
        if (this->itemData(i).toString() == p->uuid())
        {
            this->setCurrentIndex(i);
        }
    }
}
