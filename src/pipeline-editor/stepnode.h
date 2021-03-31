#ifndef STEPNODE_H
#define STEPNODE_H

#include "objectnode.h"

#include "ramstep.h"
#include "mainwindow.h"

class StepNode : public ObjectNode
{
public:
    StepNode(RamStep *step);
};

#endif // STEPNODE_H
