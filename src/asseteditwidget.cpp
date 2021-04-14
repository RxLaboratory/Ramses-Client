#include "asseteditwidget.h"

AssetEditWidget::AssetEditWidget(RamAsset *asset, QWidget *parent) :
    ObjectEditWidget(asset, parent)
{
    setupUi();
    connectEvents();

    setAsset(asset);
}

RamAsset *AssetEditWidget::asset() const
{
    return _asset;
}

void AssetEditWidget::setAsset(RamAsset *asset)
{
    this->setEnabled(false);

    setObject(asset);
    _asset = asset;

    QSignalBlocker b1(tagsEdit);
    QSignalBlocker b2(folderWidget);
    QSignalBlocker b3(assetGroupBox);

    //Reset values
    tagsEdit->setText("");
    folderWidget->setPath("");
    assetGroupBox->clear();

    if (!asset) return;

    tagsEdit->setText(asset->tags().join(", "));
    folderWidget->setPath(Ramses::instance()->path(asset));

    // Load asset groups
    RamProject *proj = project();
    if (!proj) return;

    for( RamAssetGroup *ag: proj->assetGroups() )
    {
        newAssetGroup(ag);
        if (_asset->assetGroupUuid() == ag->uuid())
            assetGroupBox->setCurrentIndex( assetGroupBox->count() -1 );
    }

    _objectConnections << connect(asset, &RamAsset::changed, this, &AssetEditWidget::assetChanged);
    _objectConnections << connect(proj,SIGNAL(assetGroupRemoved(QString)), this, SLOT(assetGroupRemoved(QString)));
    _objectConnections << connect(proj, &RamProject::newAssetGroup, this, &AssetEditWidget::newAssetGroup);

    this->setEnabled(Ramses::instance()->isLead());
}

void AssetEditWidget::update()
{
    if (!_asset) return;

    updating = true;

    _asset->setTags(tagsEdit->text());
    ObjectEditWidget::update();

    updating = false;
}

bool AssetEditWidget::checkInput()
{
    if (! ObjectEditWidget::checkInput() ) return false;

    if (assetGroupBox->currentIndex() < 0) return false;

    return true;
}

void AssetEditWidget::moveAsset()
{
    if (!_asset) return;
    RamProject *proj = project();
    if (!proj) return;
    if (assetGroupBox->currentIndex() >= 0)
        proj->moveAssetToGroup(_asset, assetGroupBox->currentData().toString());
}

void AssetEditWidget::assetChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setAsset(_asset);
}

void AssetEditWidget::newAssetGroup(RamAssetGroup *ag)
{
    assetGroupBox->addItem(ag->name(), ag->uuid());
    connect(ag, &RamAssetGroup::changed, this, &AssetEditWidget::assetGroupChanged);
}

void AssetEditWidget::assetGroupChanged(RamObject *o)
{
    for(int i = 0; i < assetGroupBox->count(); i++)
    {
        if (assetGroupBox->itemData(i).toString() == o->uuid()) assetGroupBox->setItemText(i, o->name());
    }
}

void AssetEditWidget::assetGroupRemoved(QString uuid)
{
    for(int i = assetGroupBox->count()-1; i >=0; i--)
    {
        if (assetGroupBox->itemData(i).toString() == uuid) assetGroupBox->removeItem(i);
    }
}

void AssetEditWidget::setupUi()
{
    // Tags
    QLabel *tagsLabel = new QLabel("Tags", this);
    mainFormLayout->addWidget(tagsLabel, 2, 0);

    tagsEdit = new QLineEdit(this);
    tagsEdit->setPlaceholderText("tag1, tag2, ...");
    mainFormLayout->addWidget(tagsEdit, 2, 1);

    QLabel *assetGroupLabel = new QLabel("Asset group", this);
    mainFormLayout->addWidget(assetGroupLabel, 3, 0);

    assetGroupBox = new QComboBox(this);
    mainFormLayout->addWidget(assetGroupBox, 3, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    mainLayout->addStretch();
}

void AssetEditWidget::connectEvents()
{
    connect(tagsEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(assetGroupBox, SIGNAL(currentIndexChanged(int)), this, SLOT(moveAsset()));
}

RamAssetGroup *AssetEditWidget::assetGroup()
{
    if (!_asset) return nullptr;
    return Ramses::instance()->assetGroup( _asset->assetGroupUuid() );
}

RamProject *AssetEditWidget::project()
{
    if (!_asset) return nullptr;
    RamAssetGroup *ag = assetGroup();
    if (!ag) return nullptr;
    return Ramses::instance()->project( ag->projectUuid() );
}
