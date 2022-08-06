#ifndef ASSETLISTMANAGERWIDGET_H
#define ASSETLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"

class AssetListManagerWidget : public ObjectListManagerWidget<RamItem*,RamAssetGroup*>
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
