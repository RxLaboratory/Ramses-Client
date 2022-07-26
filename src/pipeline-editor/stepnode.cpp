#include "stepnode.h"

StepNode::StepNode(RamStep *step): ObjectNode(step)
{
    _step = step;

    stepChanged();

    connect( step, &RamStep::dataChanged, this, &StepNode::stepChanged);
    connect( this, &StepNode::removed, step, &RamStep::remove);
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
    }
    else if (_step->type() == RamStep::ShotProduction)
    {
        this->setIcon(":/icons/shot");
    }
    else if (_step->type() == RamStep::PreProduction)
    {
        this->setIcon(":/icons/project");
    }
    else if (_step->type() == RamStep::PostProduction)
    {
        this->setIcon(":/icons/film");
    }
    this->setTitleColor(_step->color());
}
