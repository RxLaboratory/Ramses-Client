#include "ramstatushistory.h"

RamStatusHistory::RamStatusHistory(QObject *parent):
    RamObjectList(parent)
{

}

bool statusSorter(RamObject *a, RamObject *b)
{
    RamStatus *as = (RamStatus*)a;
    RamStatus *bs = (RamStatus*)b;
    if (as->date() != bs->date()) return as->date() < bs->date();
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStatusHistory::sort()
{
    std::sort(m_objects.begin(), m_objects.end(), statusSorter);
}
