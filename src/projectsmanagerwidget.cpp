#include "projectsmanagerwidget.h"

ProjectsManagerWidget::ProjectsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    projectWidget = new ProjectEditWidget(this);
    this->setWidget(projectWidget);
    projectWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    foreach(RamProject *project, Ramses::instance()->projects()) newProject(project);

    connect(Ramses::instance(), &Ramses::newProject, this, &ProjectsManagerWidget::newProject);
}

void ProjectsManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamProject *project, Ramses::instance()->projects())
    {
        if (project->uuid() == data.toString())
        {
            projectWidget->setProject(project);
            projectWidget->setEnabled(true);
            return;
        }
    }
    projectWidget->setEnabled(false);
}

void ProjectsManagerWidget::createItem()
{
    Ramses::instance()->createProject();
}

void ProjectsManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeProject(data.toString());
}

void ProjectsManagerWidget::newProject(RamProject *project)
{
    if (project->uuid() != "")
    {
        QListWidgetItem *projectItem = new QListWidgetItem(project->name());
        projectItem->setData(Qt::UserRole, project->uuid());
        this->addItem(projectItem);
        connect(project, &RamProject::destroyed, this, &ProjectsManagerWidget::removeProject);
        connect(project, &RamProject::changed, this, &ProjectsManagerWidget::projectChanged);
    }
}

void ProjectsManagerWidget::removeProject(QObject *project)
{
    RamProject *p = (RamProject*)project;

    removeData(p->uuid());
}

void ProjectsManagerWidget::projectChanged()
{
    RamProject *project = (RamProject*)QObject::sender();
    updateItem(project->uuid(), project->name());
}
