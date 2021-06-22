#ifndef RAMSTATUSHISTORY_H
#define RAMSTATUSHISTORY_H

#include "ramobjectlist.h"
#include "ramstatus.h"

class RamStepStatusHistory : public RamObjectList
{
    Q_OBJECT
public:
    RamStepStatusHistory(RamStep *step, RamItem *item);
    RamItem *item() const;
    RamStep *step() const;

    // MODEL REIMPLEMENTATION
    virtual void sort(int column = 0, Qt::SortOrder order = Qt::AscendingOrder) override;

private:
    RamStep *m_step;
    RamItem *m_item;
};

bool statusSorter(RamObject *a, RamObject *b);

#endif // RAMSTATUSHISTORY_H
