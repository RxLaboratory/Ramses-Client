#include "asseteditwidget.h"

AssetEditWidget::AssetEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _asset = nullptr;
    _project = nullptr;

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &AssetEditWidget::checkInput);
    connect(assetGroupBox, SIGNAL(currentIndexChanged(int)), this, SLOT(checkInput()));
    connect(DBInterface::instance(),&DBInterface::log, this, &AssetEditWidget::dbiLog);

    this->setEnabled(false);
}

RamAsset *AssetEditWidget::asset() const
{
    return _asset;
}

void AssetEditWidget::setAsset(RamAsset *asset)
{
    while (_assetConnections.count() > 0) disconnect( _assetConnections.takeLast() );

    _asset = asset;

    nameEdit->setText("");
    shortNameEdit->setText("");
    tagsEdit->setText("");

    this->setEnabled(false);

    if (!asset) return;
    if (!_project) return;

    nameEdit->setText(asset->name());
    shortNameEdit->setText(asset->shortName());
    tagsEdit->setText(asset->tags().join(", "));

    // set group
    assetGroupBox->setCurrentIndex(-1);

    for (int i = 0; i < assetGroupBox->count(); i++)
    {
        if (assetGroupBox->itemData(i).toString() == asset->assetGroupUuid())
        {
            assetGroupBox->setCurrentIndex(i);
            break;
        }
    }

    _assetConnections << connect(asset, &RamAsset::destroyed, this, &AssetEditWidget::assetDestroyed);

    this->setEnabled(Ramses::instance()->isLead());
}

void AssetEditWidget::setProject(RamProject *project)
{
    while(_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    _project = project;

    this->setEnabled(false);
    assetGroupBox->clear();

    if (!project) return;

    foreach(RamAssetGroup *ag, project->assetGroups()) newAssetGroup(ag);

    connect(project, &RamProject::assetGroupRemoved, this, &AssetEditWidget::assetGroupRemoved);
    connect(project, &RamProject::newAssetGroup, this, &AssetEditWidget::newAssetGroup);

    this->setEnabled(true);
}

void AssetEditWidget::update()
{
    if (!_asset) return;
    if (!_project) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _asset->setName(nameEdit->text());
    _asset->setShortName(shortNameEdit->text());
    _asset->setTags(tagsEdit->text());
    _asset->setAssetGroupUuid(assetGroupBox->currentData().toString());

    _asset->update();

    this->setEnabled(true);
}

void AssetEditWidget::revert()
{
    setAsset(_asset);
}

bool AssetEditWidget::checkInput()
{
    if (!_asset) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    if (assetGroupBox->currentIndex() < 0)
    {
        statusLabel->setText("Assets must be assigned to an Asset group.");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void AssetEditWidget::assetDestroyed(QObject */*o*/)
{
    setAsset(nullptr);
}

void AssetEditWidget::newAssetGroup(RamAssetGroup *ag)
{
    assetGroupBox->addItem(ag->name(), ag->uuid());
    connect(ag, &RamAssetGroup::changed, this, &AssetEditWidget::assetGroupChanged);
}

void AssetEditWidget::assetGroupChanged()
{
    RamAssetGroup *ag = (RamAssetGroup*)sender();

    for(int i = 0; i < assetGroupBox->count(); i++)
    {
        if (assetGroupBox->itemData(i).toString() == ag->uuid()) assetGroupBox->setItemText(i, ag->name());
    }
}

void AssetEditWidget::assetGroupRemoved(QString uuid)
{
    for(int i = assetGroupBox->count()-1; i >=0; i--)
    {
        if (assetGroupBox->itemData(i).toString() == uuid) assetGroupBox->removeItem(i);
    }
}

void AssetEditWidget::dbiLog(QString m, LogUtils::LogType t)
{
    if (t != LogUtils::Remote && t != LogUtils::Debug) statusLabel->setText(m);
}
