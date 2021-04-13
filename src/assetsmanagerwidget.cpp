#include "assetsmanagerwidget.h"

AssetsManagerWidget::AssetsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    assetWidget = new AssetEditWidget(this);
    this->setWidget(assetWidget);

    this->setRole(RamUser::Lead);

    changeProject(Ramses::instance()->currentProject());

    connect(Ramses::instance(), &Ramses::projectChanged, this, &AssetsManagerWidget::changeProject);
    connect(this, &ListManagerWidget::filterChanged, this, &AssetsManagerWidget::filter);
}

void AssetsManagerWidget::currentDataChanged(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    RamAsset *asset = project->asset( data.toString() );

    if (!asset) return;

    assetWidget->setAsset(asset);
}

void AssetsManagerWidget::createItem()
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    if (project->assetGroups().count() == 0 ) return;
    RamAssetGroup *ag = project->assetGroup(currentFilter());
    if (!ag) ag = project->assetGroups().at(0);
    ag->createAsset();
}

void AssetsManagerWidget::removeItem(QVariant data)
{
    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;
    project->removeAsset(data.toString());
}

void AssetsManagerWidget::changeProject(RamProject *project)
{
    this->setEnabled(false);

    while (_projectConnections.count() > 0) disconnect( _projectConnections.takeLast() );

    // empty list
    list->blockSignals(true);
    filterBox->blockSignals(true);

    list->clear();
    assetWidget->setAsset(nullptr);
    this->clearFilters();

    list->blockSignals(false);

    if (!project) return;

    this->addFilter("All assets", "all");

    // add filters
    foreach(RamAssetGroup *assetGroup, project->assetGroups()) newAssetGroup(assetGroup);

    filter("all");

    filterBox->blockSignals(false);

    _projectConnections << connect(project, SIGNAL(assetGroupRemoved(QString)), this, SLOT(assetGroupRemoved(QString)));
    _projectConnections << connect(project, &RamProject::newAssetGroup, this, &AssetsManagerWidget::newAssetGroup);

    this->setEnabled(true);
}

void AssetsManagerWidget::newAsset(RamAsset *asset)
{
    if (!asset) return;
    if (asset->uuid() != "")
    {
        QListWidgetItem *assetItem = new QListWidgetItem(asset->name());
        assetItem->setData(Qt::UserRole, asset->uuid());
        this->addItem(assetItem);
        connect(asset, &RamAsset::changed, this, &AssetsManagerWidget::assetChanged);
    }
}

void AssetsManagerWidget::assetChanged()
{
    RamAsset *a = (RamAsset*)QObject::sender();
    updateItem(a->uuid(), a->name());
}

void AssetsManagerWidget::assetRemoved(QString uuid)
{
    removeData(uuid);
}

void AssetsManagerWidget::filter(QString assetGroupUuid)
{
    while (_assetGroupsConnections.count() > 0) disconnect( _assetGroupsConnections.takeLast() );

    list->blockSignals(true);
    list->clear();
    list->blockSignals(false);

    RamProject *project = Ramses::instance()->currentProject();
    if (!project) return;

    foreach(RamAssetGroup *assetGroup, project->assetGroups())
        if (assetGroupUuid == "all" || assetGroupUuid == assetGroup->uuid()) addAssets(assetGroup);
}

void AssetsManagerWidget::addAssets(RamAssetGroup *assetGroup)
{
    foreach(RamAsset *asset, assetGroup->assets())
    {
        newAsset(asset);
    }
    _assetGroupsConnections << connect(assetGroup, &RamAssetGroup::newAsset, this, &AssetsManagerWidget::newAsset);
    _assetGroupsConnections << connect(assetGroup, SIGNAL(assetRemoved(QString)), this, SLOT(assetRemoved(QString)));
}

void AssetsManagerWidget::assetGroupChanged()
{
    RamAssetGroup *assetGroup = (RamAssetGroup*)sender();
    this->updateFilterName(assetGroup->name(), assetGroup->uuid());
}

void AssetsManagerWidget::assetGroupRemoved(QString uuid)
{
    this->removeFilter(uuid);
}

void AssetsManagerWidget::newAssetGroup(RamAssetGroup *assetGroup)
{
    _projectConnections << connect(assetGroup, &RamAssetGroup::changed, this, &AssetsManagerWidget::assetGroupChanged);
    this->addFilter(assetGroup->name(), assetGroup->uuid());
}
