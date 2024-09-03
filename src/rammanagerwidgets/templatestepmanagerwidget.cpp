#include "templatestepmanagerwidget.h"

#include "ramses.h"
#include "duwidgets/duicon.h"

TemplateStepManagerWidget::TemplateStepManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->templateSteps(),
        "Template steps",
        DuIcon(":icons/step"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamTemplateStep *TemplateStepManagerWidget::createObject()
{
    RamTemplateStep *ts = new RamTemplateStep(
                "NEW",
                "New Template Step"
                );

    ts->edit();
    return ts;
}
