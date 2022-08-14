#ifndef ASSETLISTMANAGERWIDGET_H
#define ASSETLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramitem.h"

class AssetListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamItem *createObject() override;

private slots:
    void changeProject(RamProject *project);
};

#endif // ASSETLISTMANAGERWIDGET_H
