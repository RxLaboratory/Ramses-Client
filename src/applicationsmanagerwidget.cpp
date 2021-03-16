#include "applicationsmanagerwidget.h"

ApplicationsManagerWidget::ApplicationsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
        applicationWidget = new ApplicationEditWidget(this);
        this->setWidget(applicationWidget);
        applicationWidget->setEnabled(false);

        this->setRole(RamUser::ProjectAdmin);

        foreach(RamApplication *a, Ramses::instance()->applications()) newApplication(a);

        connect(Ramses::instance(), &Ramses::newApplication, this, &ApplicationsManagerWidget::newApplication);
}

void ApplicationsManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamApplication *a, Ramses::instance()->applications())
    {
        if (a->uuid() == data.toString())
        {
            applicationWidget->setApplication(a);
            applicationWidget->setEnabled(true);
            return;
        }
    }
    applicationWidget->setEnabled(false);
}

void ApplicationsManagerWidget::createItem()
{
    Ramses::instance()->createApplication();
}

void ApplicationsManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeApplication(data.toString());
}

void ApplicationsManagerWidget::newApplication(RamApplication *a)
{
    if (a->uuid() != "")
    {
        QListWidgetItem *appItem = new QListWidgetItem(a->name());
        appItem->setData(Qt::UserRole, a->uuid());
        this->addItem(appItem);
        connect(a, &RamApplication::removed, this, &ApplicationsManagerWidget::removeApplication);
        connect(a, &RamApplication::changed, this, &ApplicationsManagerWidget::applicationChanged);
    }
}

void ApplicationsManagerWidget::removeApplication(RamObject *o)
{
    removeData(o->uuid());
}

void ApplicationsManagerWidget::applicationChanged()
{
    RamApplication *a = (RamApplication*)QObject::sender();
    updateItem(a->uuid(), a->name());
}
