#ifndef RAMSTATUSHISTORY_H
#define RAMSTATUSHISTORY_H

#include "ramobjectlist.h"
#include "ramstatus.h"

class RamStepStatusHistory : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStepStatusHistory(RamStep *step, RamItem *item);

    RamItem *item() const;
    RamStep *step() const;

public slots:
    void sort() Q_DECL_OVERRIDE;

private:
    RamStep *_step;
    RamItem *_item;
};

bool statusSorter(RamObject *a, RamObject *b);

#endif // RAMSTATUSHISTORY_H
