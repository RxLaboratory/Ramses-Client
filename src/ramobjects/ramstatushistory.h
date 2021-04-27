#ifndef RAMSTATUSHISTORY_H
#define RAMSTATUSHISTORY_H

#include "ramobjectlist.h"
#include "ramstatus.h"

class RamStatusHistory : public RamObjectList
{
    Q_OBJECT
public:
    explicit RamStatusHistory(QObject *parent = nullptr);
    RamStatus *fromUuid(QString uuid) const Q_DECL_OVERRIDE;
    RamStatus *at(int i) const Q_DECL_OVERRIDE;
    RamStatus *takeAt(int i) Q_DECL_OVERRIDE;
    RamStatus *operator[](int i) const Q_DECL_OVERRIDE;
public slots:
    void sort() Q_DECL_OVERRIDE;
};

bool statusSorter(RamObject *a, RamObject *b);

#endif // RAMSTATUSHISTORY_H
