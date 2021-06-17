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
    Ramses::instance()->createTemplateStep();
}
