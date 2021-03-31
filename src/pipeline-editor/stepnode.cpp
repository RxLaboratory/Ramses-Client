#include "stepnode.h"

StepNode::StepNode(RamStep *step): ObjectNode(step)
{
    StepEditWidget *stepWidget = new StepEditWidget(step);
    setEditWidget(stepWidget);
}
