#include "stepnode.h"

StepNode::StepNode(RamStep *step): ObjectNode(step)
{
    _step = step;


    StepEditWidget *stepWidget = new StepEditWidget(step);
    setEditWidget(stepWidget);

    stepChanged();

    connect(step, &RamStep::changed, this, &StepNode::stepChanged);
}

RamStep *StepNode::step() const
{
    return _step;
}

void StepNode::stepChanged()
{
    if (_step->type() == RamStep::AssetProduction)
    {
        this->setIcon(":/icons/asset");
        this->dockWidget()->setIcon(":/icons/asset");
    }
    else if (_step->type() == RamStep::ShotProduction)
    {
        this->setIcon(":/icons/shot");
        this->dockWidget()->setIcon(":/icons/shot");
    }
    else if (_step->type() == RamStep::PreProduction)
    {
        this->setIcon(":/icons/project");
        this->dockWidget()->setIcon(":/icons/project");
    }
    else if (_step->type() == RamStep::PostProduction)
    {
        this->setIcon(":/icons/film");
        this->dockWidget()->setIcon(":/icons/film");
    }
}
