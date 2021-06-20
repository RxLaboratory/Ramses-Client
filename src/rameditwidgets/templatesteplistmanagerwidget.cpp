#include "templatesteplistmanagerwidget.h"

TemplateStepListManagerWidget::TemplateStepListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateSteps(),
        new TemplateStepEditWidget(),
        "Template steps",
        parent )
{
    this->setContainingType(RamObject::Step);
}

void TemplateStepListManagerWidget::createObject()
{
    RamStep *ts = new RamStep(
                "NEW",
                "New Template Step"
                );

    Ramses::instance()->templateSteps()->append(ts);
}
