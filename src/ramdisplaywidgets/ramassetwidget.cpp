#include "ramassetwidget.h"

RamAssetWidget::RamAssetWidget(RamAsset *asset, QWidget *parent) :
    RamObjectWidget(asset, parent)
{
    _asset = asset;

    AssetEditWidget *aw = new AssetEditWidget(this);
    aw->setAsset(asset);
    setEditWidget(aw);

    setUserEditRole(RamUser::Lead);

    setIcon(":/icons/asset");
}

RamAsset *RamAssetWidget::asset() const
{
    return _asset;
}
