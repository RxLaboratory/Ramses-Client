#include "ramstepstatushistory.h"

#include "ramitem.h"

RamStepStatusHistory::RamStepStatusHistory(RamStep *step, RamItem *item):
    RamObjectList(step->shortName(), step->name(), item)
{
    m_item = item;
    m_step = step;

    this->setObjectName( "RamStepStatusHistory " + step->shortName() );
}

RamItem *RamStepStatusHistory::item() const
{
    return m_item;
}

RamStep *RamStepStatusHistory::step() const
{
    return m_step;
}

bool statusSorter(RamObject *a, RamObject *b)
{
    RamStatus *as = qobject_cast<RamStatus*>(a);
    RamStatus *bs = qobject_cast<RamStatus*>(b);
    if (as->date() != bs->date()) return as->date() < bs->date();
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStepStatusHistory::sort(int column, Qt::SortOrder order)
{
    /*if (m_sorted) return;
    std::sort(m_objectsList.begin(), m_objectsList.end(), statusSorter);
    m_sorted = false;*/
}
