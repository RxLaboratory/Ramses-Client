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
    this->setIcon(_step->iconName());
    this->setTitleColor(_step->color());
}
