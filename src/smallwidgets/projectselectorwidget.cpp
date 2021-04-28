#include "projectselectorwidget.h"

ProjectSelectorWidget::ProjectSelectorWidget(QWidget *parent): QComboBox(parent)
{
    //Populate
    for (int i = 0; i < Ramses::instance()->projects()->count(); i++) newProject( Ramses::instance()->projects()->at(i) );

    this->setMinimumWidth(200);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentProject(int)));
    connect(Ramses::instance()->projects(), &RamObjectList::objectAdded, this, &ProjectSelectorWidget::newProject);
    connect(Ramses::instance()->projects(), &RamObjectList::objectRemoved, this, &ProjectSelectorWidget::projectRemoved);
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &ProjectSelectorWidget::currentProjectChanged);
}

void ProjectSelectorWidget::newProject(RamObject *obj)
{
    QSignalBlocker b(this);
    int i = currentIndex();
    if (obj->uuid() != "")
    {
        this->addItem(obj->name(), obj->uuid());
        connect(obj, &RamObject::changed, this, &ProjectSelectorWidget::projectChanged);
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
