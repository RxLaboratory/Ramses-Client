#include "stepnode.h"

StepNode::StepNode(RamStep *step): ObjectNode(step)
{
    StepEditWidget *stepWidget = new StepEditWidget(step);
    setEditWidget(stepWidget);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addStepDockWidget(this->dockWidget());
}
