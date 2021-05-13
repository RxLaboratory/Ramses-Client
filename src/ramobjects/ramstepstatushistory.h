#ifndef RAMSTATUSHISTORY_H
#define RAMSTATUSHISTORY_H

#include "ramobjectlist.h"
#include "ramstatus.h"

class RamStepStatusHistory : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStepStatusHistory(QObject *parent = nullptr);
    explicit RamStepStatusHistory(RamStep *step, QObject *parent = nullptr);

public slots:
    void sort() Q_DECL_OVERRIDE;
};

bool statusSorter(RamObject *a, RamObject *b);

#endif // RAMSTATUSHISTORY_H
