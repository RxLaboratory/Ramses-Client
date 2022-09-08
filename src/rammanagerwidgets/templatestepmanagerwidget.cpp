#include "templatestepmanagerwidget.h"

#include "ramses.h"

TemplateStepManagerWidget::TemplateStepManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->templateSteps(),
        "Template steps",
        QIcon(":icons/step"),
        parent )
{
    m_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
}

RamTemplateStep *TemplateStepManagerWidget::createObject()
{
    RamTemplateStep *ts = new RamTemplateStep(
                "NEW",
                "New Template Step"
                );

    //Ramses::instance()->templateSteps()->append(ts);

    ts->edit();
    return ts;
}
