#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent): QComboBox(parent)
{
    //Populate
    foreach(RamProject *p, Ramses::instance()->projects()) newProject(p);

    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentProject(int)));
    connect(Ramses::instance(), &Ramses::newProject, this, &ProjectSelectorWidget::newProject);
    connect(Ramses::instance(), &Ramses::projectChanged, this, &ProjectSelectorWidget::currentProjectChanged);
}

void ProjectSelectorWidget::newProject(RamProject *project)
{
    QSignalBlocker b(this);
    int i = currentIndex();
    if (project->uuid() != "")
    {
        this->addItem(project->name(), project->uuid());
        connect(project, &RamProject::removed, this, &ProjectSelectorWidget::projectRemoved);
        connect(project, &RamProject::changed, this, &ProjectSelectorWidget::projectChanged);
    }
    setCurrentIndex(i);
}

void ProjectSelectorWidget::projectRemoved(RamObject *o)
{
    for (int i = this->count() - 1; i >= 0; i--)
    {
        if (this->itemData(i).toString() == o->uuid())
        {
            this->removeItem(i);
        }
    }
}

void ProjectSelectorWidget::projectChanged(RamObject *o)
{
    for (int i = this->count() - 1; i >= 0; i--)
    {
        if (this->itemData(i).toString() == o->uuid())
        {
            this->setItemText(i, o->name());
        }
    }
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
