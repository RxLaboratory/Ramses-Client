#include "templatesteplistmanagerwidget.h"

#include "ramses.h"

TemplateStepListManagerWidget::TemplateStepListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateSteps(),
        "Template steps",
        QIcon(":icons/step"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamTemplateStep *TemplateStepListManagerWidget::createObject()
{
    RamTemplateStep *ts = new RamTemplateStep(
                "NEW",
                "New Template Step"
                );

    Ramses::instance()->templateSteps()->append(ts);

    ts->edit();
    return ts;
}
