#include "ramstatushistory.h"

RamStatusHistory::RamStatusHistory(QObject *parent):
    RamObjectList(parent)
{

}

RamStatus *RamStatusHistory::fromUuid(QString uuid) const
{
    return (RamStatus*)RamObjectList::fromUuid(uuid);
}

RamStatus *RamStatusHistory::at(int i) const
{
    return (RamStatus*)RamObjectList::at(i);
}

RamStatus *RamStatusHistory::takeAt(int i)
{
    return (RamStatus*)RamObjectList::takeAt(i);
}

RamStatus *RamStatusHistory::operator[](int i) const
{
    return (RamStatus*)RamObjectList::operator[](i);
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
