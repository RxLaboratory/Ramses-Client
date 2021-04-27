#ifndef RAMSTATELIST_H
#define RAMSTATELIST_H

#include "ramobjectlist.h"
#include "ramstate.h"

class RamStateList : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStateList(QObject *parent = nullptr);
    RamState *fromUuid(QString uuid) const Q_DECL_OVERRIDE;
    RamState *at(int i) const Q_DECL_OVERRIDE;
    RamState *takeAt(int i) Q_DECL_OVERRIDE;
    RamState *operator[](int i) const Q_DECL_OVERRIDE;
public slots:
    void sort() Q_DECL_OVERRIDE;
};

bool stateSorter(RamObject *a, RamObject *b);

#endif // RAMSTATELIST_H
