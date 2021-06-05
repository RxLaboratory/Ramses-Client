#include "ramstepstatushistory.h"

RamStepStatusHistory::RamStepStatusHistory(QObject *parent):
    RamObjectList(parent)
{

}

RamStepStatusHistory::RamStepStatusHistory(RamStep *step, QObject *parent):
    RamObjectList(step->shortName(), step->name(), step->uuid(), parent)
{

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
