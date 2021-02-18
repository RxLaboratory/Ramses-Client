#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent): QComboBox(parent)
{
    //Populate
    foreach(RamProject *p, Ramses::instance()->projects()) newProject(p);

    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentProject(int)));
    connect(Ramses::instance(), &Ramses::newProject, this, &ProjectSelectorWidget::newProject);
}

void ProjectSelectorWidget::newProject(RamProject *project)
{
    if (project->uuid() != "")
    {
        this->addItem(project->name(), project->uuid());
        connect(project, &RamProject::destroyed, this, &ProjectSelectorWidget::removeProject);
        connect(project, &RamProject::changed, this, &ProjectSelectorWidget::projectChanged);
    }
}

void ProjectSelectorWidget::removeProject(QObject *o)
{
    RamProject *p = (RamProject*)o;
    for (int i = this->count() - 1; i >= 0; i--)
    {
        if (this->itemData(i).toString() == p->uuid())
        {
            this->removeItem(i);
        }
    }
}

void ProjectSelectorWidget::projectChanged()
{
    RamProject *p = (RamProject*)QObject::sender();
    for (int i = this->count() - 1; i >= 0; i--)
    {
        if (this->itemData(i).toString() == p->uuid())
        {
            this->setItemText(i, p->name());
        }
    }
}

void ProjectSelectorWidget::setCurrentProject(int index)
{
    Ramses::instance()->setCurrentProject( this->itemData(index).toString() );
}
