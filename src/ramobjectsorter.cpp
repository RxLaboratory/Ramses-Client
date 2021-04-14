#include "ramobjectsorter.h"

bool objectSorter(RamObject *a, RamObject *b)
{
    if (a->order() != b->order()) return a->order() < b->order();
    else return a->shortName() < b->shortName();
}
