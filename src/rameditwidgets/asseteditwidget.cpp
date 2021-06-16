#include "asseteditwidget.h"

AssetEditWidget::AssetEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();

    changeProject(Ramses::instance()->currentProject());

    connectEvents();
}

RamAsset *AssetEditWidget::asset() const
{
    return _asset;
}

void AssetEditWidget::setAsset(RamAsset *asset)
{
    this->setEnabled(false);

    ObjectEditWidget::setObject(asset);
    _asset = asset;

    QSignalBlocker b1(tagsEdit);
    QSignalBlocker b2(folderWidget);
    QSignalBlocker b3(assetGroupBox);

    //Reset values
    tagsEdit->setText("");
    folderWidget->setPath("");
    assetGroupBox->setCurrentIndex(-1);
    statusHistoryWidget->setItem(asset);

    if (!asset) return;

    tagsEdit->setText(asset->tags().join(", "));
    folderWidget->setPath(Ramses::instance()->path(asset));

    //set asset group
    assetGroupBox->setObject( _asset->assetGroup() );

    this->setEnabled(Ramses::instance()->isLead());
}

void AssetEditWidget::setObject(RamObject *obj)
{
    RamAsset *asset = qobject_cast<RamAsset*>(obj);
    setAsset(asset);
}

void AssetEditWidget::update()
{
    if (!_asset) return;

    if (!checkInput()) return;

    updating = true;

    _asset->setTags(tagsEdit->text());
    ObjectEditWidget::update();

    updating = false;
}

void AssetEditWidget::changeProject(RamProject *project)
{
    setAsset(nullptr);
    if (!project)
    {
        assetGroupBox->setList(nullptr);
        return;
    }
    assetGroupBox->setList(project->assetGroups());
}

void AssetEditWidget::moveAsset()
{
    if (!_asset) return;
    RamProject *proj = project();
    if (!proj) return;
    if (assetGroupBox->currentIndex() >= 0)
        proj->moveAssetToGroup(_asset, assetGroupBox->currentUuid() );
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

    assetGroupBox = new RamObjectListComboBox(this);
    mainFormLayout->addWidget(assetGroupBox, 3, 1);

    folderWidget = new DuQFFolderDisplayWidget(this);
    mainLayout->insertWidget(1, folderWidget);

    statusHistoryWidget = new StatusHistoryWidget( this );
    mainLayout->addWidget(statusHistoryWidget);
}

void AssetEditWidget::connectEvents()
{
    connect(tagsEdit, SIGNAL(editingFinished()), this, SLOT(update()));
    connect(assetGroupBox, SIGNAL(currentIndexChanged(int)), this, SLOT(moveAsset()));
    connect(Ramses::instance(), &Ramses::currentProjectChanged, this, &AssetEditWidget::changeProject);
}

RamAssetGroup *AssetEditWidget::assetGroup()
{
    if (!_asset) return nullptr;
    return _asset->assetGroup();
}

RamProject *AssetEditWidget::project()
{
    if (!_asset) return nullptr;
    return _asset->project();
}
