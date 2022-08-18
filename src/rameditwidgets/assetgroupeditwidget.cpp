#include "assetgroupeditwidget.h"

#include "ramasset.h"
#include "ramproject.h"
#include "data-models/ramitemtable.h"

AssetGroupEditWidget::AssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

AssetGroupEditWidget::AssetGroupEditWidget(RamAssetGroup *ag, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(ag);
}

RamAssetGroup *AssetGroupEditWidget::assetGroup() const
{
    return m_assetGroup;
}

void AssetGroupEditWidget::reInit(RamObject *o)
{
    m_assetGroup = qobject_cast<RamAssetGroup*>(o);
    if (m_assetGroup)
    {
        ui_assetsList->setList(m_assetGroup->project()->assets());
        ui_assetsList->setFilter(m_assetGroup);

        ui_folderWidget->setPath(m_assetGroup->path());
    }
    else
    {
        ui_folderWidget->setPath("");
        ui_assetsList->setList(nullptr);
    }
}

void AssetGroupEditWidget::createAsset()
{
    if (!m_assetGroup) return;
    RamAsset *asset = new RamAsset(
                "NEW",
                "New Asset",
                m_assetGroup);
    m_assetGroup->project()->assets()->append(asset);
    asset->edit();
}

void AssetGroupEditWidget::setupUi()
{
    ui_folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainLayout->insertWidget(1, ui_folderWidget);

    ui_assetsList = new ObjectListEditWidget(true, RamUser::ProjectAdmin, this);
    ui_assetsList->setEditMode(ObjectListEditWidget::RemoveObjects);
    ui_assetsList->setTitle("Assets");
    ui_mainLayout->addWidget(ui_assetsList);
}

void AssetGroupEditWidget::connectEvents()
{
    connect(ui_assetsList, SIGNAL(add()), this, SLOT(createAsset()));
}
