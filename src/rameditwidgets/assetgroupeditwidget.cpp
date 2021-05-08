#include "assetgroupeditwidget.h"

AssetGroupEditWidget::AssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setAssetGroup(nullptr);
}

AssetGroupEditWidget::AssetGroupEditWidget(RamAssetGroup *ag, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
    setAssetGroup(ag);}

RamAssetGroup *AssetGroupEditWidget::assetGroup() const
{
    return _assetGroup;
}

void AssetGroupEditWidget::setAssetGroup(RamAssetGroup *assetGroup)
{
    this->setEnabled(false);

    setObject(assetGroup);
    _assetGroup = assetGroup;

    QSignalBlocker b1(assetsList);

    //Reset values
    folderWidget->setPath("");
    assetsList->clear();

    if (!assetGroup) return;

    folderWidget->setPath(Ramses::instance()->path(assetGroup));

    // Load assets
    foreach(RamAsset *a, assetGroup->assets()) newAsset(a);

    _objectConnections << connect(assetGroup, &RamAssetGroup::changed, this, &AssetGroupEditWidget::assetGroupChanged);
    _objectConnections << connect(assetGroup, &RamAssetGroup::newAsset, this, &AssetGroupEditWidget::newAsset);
    _objectConnections << connect(assetGroup, &RamAssetGroup::assetRemovedFromGroup, this, &AssetGroupEditWidget::assetRemoved);

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

void AssetGroupEditWidget::assetGroupChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setAssetGroup(_assetGroup);
}

void AssetGroupEditWidget::newAsset(RamAsset *a)
{
    assetsList->addObject(a, _creatingAsset);
    _creatingAsset = false;
}

void AssetGroupEditWidget::assetRemoved(RamAsset *asset)
{
    assetsList->removeObject(asset);
}

void AssetGroupEditWidget::addAsset()
{
    if (!_assetGroup) return;
    _creatingAsset = true;
    _assetGroup->createAsset();
}

void AssetGroupEditWidget::removeAsset(RamObject *o)
{
    if (!_assetGroup) return;
    o->remove();
}

void AssetGroupEditWidget::setupUi()
{
    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    assetsList = new SimpleObjectList(true, this);
    assetsList->setTitle("Assets");
    mainLayout->addWidget(assetsList);
}

void AssetGroupEditWidget::connectEvents()
{
    connect(assetsList, &SimpleObjectList::add, this, &AssetGroupEditWidget::addAsset);
    connect(assetsList, &SimpleObjectList::objectRemoved, this, &AssetGroupEditWidget::removeAsset);
}