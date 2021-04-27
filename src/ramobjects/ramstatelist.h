#ifndef RAMSTATELIST_H
#define RAMSTATELIST_H

#include "ramobjectlist.h"
#include "ramstate.h"

class RamStateList : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStateList(QObject *parent = nullptr);
public slots:
    void sort() Q_DECL_OVERRIDE;
};

bool stateSorter(RamObject *a, RamObject *b);

#endif // RAMSTATELIST_H
