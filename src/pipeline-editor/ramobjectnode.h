#ifndef STEPNODE_H
#define STEPNODE_H

#include "duqf-nodeview/duqfnode.h"

#include "ramstep.h"

class RamObjectNode : public DuQFNode
{
public:
    RamObjectNode(RamObject *obj);
    RamObject *ramObject() const;

private slots:
    void objectChanged();
    void stepRemoved();

private:
    RamObject *_object;
};

#endif // STEPNODE_H
