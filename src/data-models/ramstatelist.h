#ifndef RAMSTATELIST_H
#define RAMSTATELIST_H

#include "ramobjectlist.h"
#include "ramstate.h"

class RamStateList : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStateList(QObject *parent = nullptr);

    virtual void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) override;
};

bool stateSorter(RamObject *a, RamObject *b);

#endif // RAMSTATELIST_H
