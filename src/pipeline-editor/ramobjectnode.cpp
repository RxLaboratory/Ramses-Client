#include "ramobjectnode.h"

RamObjectNode::RamObjectNode(RamObject *obj)
{
    _object = obj;
    this->setTitle(obj->shortName());
    connect(obj, &RamStep::changed, this, &RamObjectNode::objectChanged);
    connect(obj, &RamStep::removed, this, &RamObjectNode::stepRemoved);
}

RamObject *RamObjectNode::ramObject() const
{
    return _object;
}

void RamObjectNode::objectChanged()
{
    this->setTitle(_object->shortName());
}

void RamObjectNode::stepRemoved()
{
    this->remove();
}
