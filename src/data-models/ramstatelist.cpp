#include "ramstatelist.h"

RamStateList::RamStateList(QObject *parent):
    RamObjectList("STATES", "States", parent)
{
    this->setObjectName( "RamStateList" );
}

bool stateSorter(RamObject *a, RamObject *b)
{
    RamState *as = (RamState*)a;
    RamState *bs = (RamState*)b;
    if (as->completionRatio() != bs->completionRatio()) return as->completionRatio() < bs->completionRatio();
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStateList::sort(int column, Qt::SortOrder order)
{
    /*if (m_sorted) return;
    std::sort(m_objectsList.begin(), m_objectsList.end(), stateSorter);
    m_sorted = true;*/
}
