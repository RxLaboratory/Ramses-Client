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
    return _assetGroup;
}

void AssetGroupEditWidget::setObject(RamObject *obj)
{
    RamAssetGroup *assetGroup = qobject_cast<RamAssetGroup*>(obj);

    this->setEnabled(false);

    ObjectEditWidget::setObject(assetGroup);
    _assetGroup = assetGroup;

    QSignalBlocker b1(assetsList);

    //Reset values
    folderWidget->setPath("");
    //assetsList->setList(assetGroup);

    if (!assetGroup) return;

    folderWidget->setPath(Ramses::instance()->path(assetGroup));

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void AssetGroupEditWidget::update()
{
    if (!_assetGroup) return;

    updating = true;

    if (!checkInput()) return;

    _assetGroup->setName(nameEdit->text());
    _assetGroup->setShortName(shortNameEdit->text());

    _assetGroup->update();

    updating = false;
}

void AssetGroupEditWidget::createAsset()
{
    if (!_assetGroup) return;
    RamAsset *asset = new RamAsset(
                "NEW",
                _assetGroup,
                "New Asset");
    _assetGroup->project()->assets()->append(asset);
}

void AssetGroupEditWidget::setupUi()
{
    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    assetsList = new ObjectListEditWidget(true, this);
    assetsList->setTitle("Assets");
    mainLayout->addWidget(assetsList);
}

void AssetGroupEditWidget::connectEvents()
{
    connect(assetsList, &ObjectListEditWidget::add, this, &AssetGroupEditWidget::createAsset);
}
