#include "asseteditwidget.h"

AssetEditWidget::AssetEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    connectEvents();
}

AssetEditWidget::AssetEditWidget(RamAsset *asset, QWidget *parent)
{
    setupUi();
    connectEvents();

    setObject(asset);
}

RamAsset *AssetEditWidget::asset() const
{
    return _asset;
}

void AssetEditWidget::setObject(RamObject *obj)
{
    this->setEnabled(false);

    RamAsset *asset = qobject_cast<RamAsset*>(obj);

    ObjectEditWidget::setObject(asset);
    _asset = asset;

    QSignalBlocker b1(tagsEdit);
    QSignalBlocker b2(folderWidget);
    QSignalBlocker b3(assetGroupBox);

    //Reset values
    tagsEdit->setText("");
    folderWidget->setPath("");
    assetGroupBox->setCurrentIndex(-1);

    if (!asset) return;

    tagsEdit->setText(asset->tags().join(", "));
    folderWidget->setPath(Ramses::instance()->path(asset));

    //set asset group
    RamProject *project = _asset->project();
    assetGroupBox->setList( project->assetGroups() );
    assetGroupBox->setObject( _asset->assetGroup() );

    this->setEnabled(Ramses::instance()->isLead());
}

void AssetEditWidget::update()
{
    if (!_asset) return;

    if (!checkInput()) return;

    updating = true;

    _asset->setTags(tagsEdit->text());
    RamAssetGroup *ag = qobject_cast<RamAssetGroup*>( assetGroupBox->currentObject() );
    _asset->setAssetGroup( ag );
    ObjectEditWidget::update();

    updating = false;
}

void AssetEditWidget::setupUi()
{
    // Tags
    QLabel *tagsLabel = new QLabel("Tags", this);
    ui_mainFormLayout->addWidget(tagsLabel, 3, 0);

    tagsEdit = new QLineEdit(this);
    tagsEdit->setPlaceholderText("tag1, tag2, ...");
    ui_mainFormLayout->addWidget(tagsEdit, 3, 1);

    QLabel *assetGroupLabel = new QLabel("Asset group", this);
    ui_mainFormLayout->addWidget(assetGroupLabel, 4, 0);

    assetGroupBox = new RamObjectListComboBox(this);
    ui_mainFormLayout->addWidget(assetGroupBox, 4, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    ui_mainLayout->insertWidget(1, folderWidget);

    ui_mainLayout->addStretch();
}

void AssetEditWidget::connectEvents()
{
    connect(tagsEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(assetGroupBox, SIGNAL(currentIndexChanged(int)), this, SLOT(update()));
}
