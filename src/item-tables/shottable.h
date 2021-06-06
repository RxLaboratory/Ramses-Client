#ifndef SHOTTABLE_H
#define SHOTTABLE_H

#include "itemtable.h"

class ShotTable : public ItemTable
{
    Q_OBJECT
public:
    ShotTable(QString title= "Items Table", QWidget *parent = nullptr);
private slots:
    void projectChanged(RamProject *project);
};

#endif // SHOTTABLE_H
