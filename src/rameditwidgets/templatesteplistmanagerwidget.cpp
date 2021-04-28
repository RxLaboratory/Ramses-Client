#include "templatesteplistmanagerwidget.h"

TemplateStepListManagerWidget::TemplateStepListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateSteps(),
        new TemplateStepEditWidget(),
        "Template steps",
        parent )
{
    qDebug() << "test";
}

void TemplateStepListManagerWidget::createObject()
{
    Ramses::instance()->createTemplateStep();
}
