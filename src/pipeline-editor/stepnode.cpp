#include "stepnode.h"

StepNode::StepNode(RamStep *step): ObjectNode(step)
{
    _step = step;

    if (step->type() == RamStep::AssetProduction)
        this->setIcon(":/icons/asset");
    else if (step->type() == RamStep::ShotProduction)
        this->setIcon(":/icons/shot");
    else if (step->type() == RamStep::PreProduction)
        this->setIcon(":/icons/project");
    else if (step->type() == RamStep::PostProduction)
        this->setIcon(":/icons/film");

    StepEditWidget *stepWidget = new StepEditWidget(step);
    setEditWidget(stepWidget);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addStepDockWidget(this->dockWidget());
}

RamStep *StepNode::step() const
{
    return _step;
}
