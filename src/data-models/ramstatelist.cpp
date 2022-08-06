#include "ramstatelist.h"

RamStateList::RamStateList(QObject *parent):
    RamObjectList<RamState*>("STATES", "States", parent, RamObjectList<RamState*>::Table)
{
    this->setObjectName( "RamStateList" );
}

bool stateSorter(RamObject *a, RamObject *b)
{
    RamState *as = (RamState*)a;
    RamState *bs = (RamState*)b;
    if (as->completionRatio() != bs->completionRatio()) return as->completionRatio() < bs->completionRatio();
    else return a->shortName() < b->shortName();
}

void RamStateList::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column)
    Q_UNUSED(order)

    std::sort(m_objectList.begin(), m_objectList.end(), stateSorter);
}
