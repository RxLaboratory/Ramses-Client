#include "templatesteplistmanagerwidget.h"

TemplateStepListManagerWidget::TemplateStepListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateSteps(),
        new TemplateStepEditWidget(),
        "Template steps",
        parent )
{

}

void TemplateStepListManagerWidget::createObject()
{
    Ramses::instance()->createTemplateStep();
}
