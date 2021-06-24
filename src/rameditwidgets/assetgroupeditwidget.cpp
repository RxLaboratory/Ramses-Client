#include "assetgroupeditwidget.h"

AssetGroupEditWidget::AssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setObject(nullptr);
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

void AssetGroupEditWidget::setObject(RamObject *obj)
{
    RamAssetGroup *assetGroup = qobject_cast<RamAssetGroup*>(obj);

    this->setEnabled(false);

    ObjectEditWidget::setObject(assetGroup);
    m_assetGroup = assetGroup;

    QSignalBlocker b1(ui_assetsList);

    //Reset values
    ui_folderWidget->setPath("");
    ui_assetsList->setList(nullptr);

    if (!assetGroup) return;

    ui_assetsList->setList(assetGroup->project()->assets());
    ui_assetsList->setFilter(assetGroup);

    ui_folderWidget->setPath(Ramses::instance()->path(assetGroup));

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void AssetGroupEditWidget::update()
{
    if (!m_assetGroup) return;

    updating = true;

    ObjectEditWidget::update();

    updating = false;
}

void AssetGroupEditWidget::createAsset()
{
    if (!m_assetGroup) return;
    RamAsset *asset = new RamAsset(
                "NEW",
                m_assetGroup,
                "New Asset");
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
