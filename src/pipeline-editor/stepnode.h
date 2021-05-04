#ifndef STEPNODE_H
#define STEPNODE_H

#include "objectnode.h"

#include "ramstep.h"
#include "stepeditwidget.h"

class StepNode : public ObjectNode
{
public:
    StepNode(RamStep *step);
    RamStep *step() const;
private slots:
    void stepChanged();
private:
    RamStep *_step;
};

#endif // STEPNODE_H
