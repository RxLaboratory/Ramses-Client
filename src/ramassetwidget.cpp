#include "ramassetwidget.h"

#include "mainwindow.h"

RamAssetWidget::RamAssetWidget(RamAsset *asset, QWidget *parent) :
    RamObjectWidget(asset, parent)
{
    _asset = asset;

    AssetEditWidget *aw = new AssetEditWidget(asset, this);
    setEditWidget(aw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addAssetEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Lead);
}

RamAsset *RamAssetWidget::asset() const
{
    return _asset;
}
