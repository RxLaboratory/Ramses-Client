#include "templatestepsmanagerwidget.h"

TemplateStepsManagerWidget::TemplateStepsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    stepWidget = new TemplateStepEditWidget(this);
    this->setWidget(stepWidget);
    stepWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    foreach(RamStep *step, Ramses::instance()->templateSteps()) newStep(step);

    connect(Ramses::instance(), &Ramses::newTemplateStep, this, &TemplateStepsManagerWidget::newStep);
}

void TemplateStepsManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamStep *step, Ramses::instance()->templateSteps())
    {
        if (step->uuid() == data.toString())
        {
            stepWidget->setStep(step);
            stepWidget->setEnabled(true);
            return;
        }
    }
    stepWidget->setEnabled(false);
}

void TemplateStepsManagerWidget::createItem()
{
    Ramses::instance()->createTemplateStep();
}

void TemplateStepsManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeTemplateStep(data.toString());
}

void TemplateStepsManagerWidget::newStep(RamStep *step)
{
    if (step->uuid() != "")
    {
        QListWidgetItem *stepItem = new QListWidgetItem(step->name());
        stepItem->setData(Qt::UserRole, step->uuid());
        this->addItem(stepItem);
        connect(step, &RamStep::removed, this, &TemplateStepsManagerWidget::stepRemoved);
        connect(step, &RamStep::changed, this, &TemplateStepsManagerWidget::stepChanged);
    }
}

void TemplateStepsManagerWidget::stepRemoved(RamObject *step)
{
    removeData(step->uuid());
}

void TemplateStepsManagerWidget::stepChanged()
{
    RamStep *step = (RamStep*)QObject::sender();
    updateItem(step->uuid(), step->name());
}
