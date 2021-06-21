#include "templatesteplistmanagerwidget.h"

TemplateStepListManagerWidget::TemplateStepListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateSteps(),
        new TemplateStepEditWidget(),
        "Template steps",
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void TemplateStepListManagerWidget::createObject()
{
    RamStep *ts = new RamStep(
                "NEW",
                "New Template Step"
                );

    Ramses::instance()->templateSteps()->append(ts);

    editObject(ts);
}
