#include "ramassetwidget.h"

#include "mainwindow.h"

RamAssetWidget::RamAssetWidget(RamAsset *asset, QWidget *parent) :
    RamObjectWidget(asset, parent)
{
    _asset = asset;

    AssetEditWidget *aw = new AssetEditWidget(asset, this);
    setEditWidget(aw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    this->dockEditWidget()->setIcon(":/icons/asset");
    mw->addAssetEditDockWidget(this->dockEditWidget());

    setUserEditRole(RamUser::Lead);

    setIcon(":/icons/asset");
}

RamAsset *RamAssetWidget::asset() const
{
    return _asset;
}
