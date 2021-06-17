#include "ramstepstatushistory.h"

#include "ramitem.h"

RamStepStatusHistory::RamStepStatusHistory(RamStep *step, RamItem *item):
    RamObjectList(step->shortName(), step->name(), "", item)
{
    this->setObjectType(StepStatusHistory);
    _item = item;
    _step = step;

    this->setObjectName( "RamStepStatusHistory" );
}

RamItem *RamStepStatusHistory::item() const
{
    return _item;
}

RamStep *RamStepStatusHistory::step() const
{
    return _step;
}


bool statusSorter(RamObject *a, RamObject *b)
{
    RamStatus *as = qobject_cast<RamStatus*>(a);
    RamStatus *bs = qobject_cast<RamStatus*>(b);
    if (as->date() != bs->date()) return as->date() < bs->date();
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStepStatusHistory::sort()
{
    std::sort(m_objectsList.begin(), m_objectsList.end(), statusSorter);
}
