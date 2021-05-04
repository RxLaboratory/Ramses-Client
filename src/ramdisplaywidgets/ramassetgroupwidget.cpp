#include "ramassetgroupwidget.h"

RamAssetGroupWidget::RamAssetGroupWidget(RamAssetGroup *assetGroup, QWidget *parent):
    RamObjectWidget(assetGroup, parent)
{
    m_assetGroup = assetGroup;

    AssetGroupEditWidget *agw = new AssetGroupEditWidget(assetGroup, this);
    setEditWidget(agw);

    setUserEditRole(RamUser::ProjectAdmin);

    if (assetGroup->isTemplate()) this->setTitle( assetGroup->name() + " [Template]");

    setIcon(":/icons/asset-group");
}


RamAssetGroup *RamAssetGroupWidget::assetGroup() const
{
    return m_assetGroup;
}
