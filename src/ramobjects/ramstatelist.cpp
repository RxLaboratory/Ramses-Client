#include "ramstatelist.h"

RamStateList::RamStateList(QObject *parent):
    RamObjectList(parent)
{

}

RamState *RamStateList::fromUuid(QString uuid) const
{
    return (RamState*)RamObjectList::fromUuid(uuid);
}

RamState *RamStateList::at(int i) const
{
    return (RamState*)RamObjectList::at(i);
}

RamState *RamStateList::takeAt(int i)
{
    return (RamState*)RamObjectList::takeAt(i);
}

RamState *RamStateList::operator[](int i) const
{
    return (RamState*)RamObjectList::operator[](i);
}

bool stateSorter(RamObject *a, RamObject *b)
{
    RamState *as = (RamState*)a;
    RamState *bs = (RamState*)b;
    if (as->completionRatio() != bs->completionRatio()) return as->completionRatio() < bs->completionRatio();
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}

void RamStateList::sort()
{
    std::sort(m_objects.begin(), m_objects.end(), stateSorter);
}
