#ifndef ASSETTABLEMANAGERWIDGET_H
#define ASSETTABLEMANAGERWIDGET_H

#include "itemtablemanagerwidget.h"

class AssetTableManagerWidget : public ItemTableManagerWidget
{
    Q_OBJECT
public:
    AssetTableManagerWidget(QString title= "Items Table", QWidget *parent = nullptr);

protected slots:
    virtual void projectChanged(RamProject *project) Q_DECL_OVERRIDE;
};

#endif // ASSETTABLEMANAGERWIDGET_H
