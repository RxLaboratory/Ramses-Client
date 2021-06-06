#ifndef ASSETTABLE_H
#define ASSETTABLE_H

#include "itemtable.h"

class AssetTable : public ItemTable
{
    Q_OBJECT
public:
    AssetTable(QString title= "Items Table", QWidget *parent = nullptr);

protected slots:
    virtual void projectChanged(RamProject *project) Q_DECL_OVERRIDE;
};

#endif // ASSETTABLE_H
