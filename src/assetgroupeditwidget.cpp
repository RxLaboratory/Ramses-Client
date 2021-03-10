#include "assetgroupeditwidget.h"

AssetGroupEditWidget::AssetGroupEditWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);

    _assetGroup = nullptr;

    connect(updateButton, SIGNAL(clicked()), this, SLOT(update()));
    connect(revertButton, SIGNAL(clicked()), this, SLOT(revert()));
    connect(shortNameEdit, &QLineEdit::textChanged, this, &AssetGroupEditWidget::checkInput);
    connect(DBInterface::instance(),&DBInterface::newLog, this, &AssetGroupEditWidget::dbiLog);

    this->setEnabled(false);
}

RamAssetGroup *AssetGroupEditWidget::assetGroup() const
{
    return _assetGroup;
}

void AssetGroupEditWidget::setAssetGroup(RamAssetGroup *assetGroup)
{
    while (_assetGroupConnections.count() > 0) disconnect( _assetGroupConnections.takeLast() );

    _assetGroup = assetGroup;

    nameEdit->setText("");
    shortNameEdit->setText("");
    this->setEnabled(false);

    if (!assetGroup) return;

    nameEdit->setText(assetGroup->name());
    shortNameEdit->setText(assetGroup->shortName());

    _assetGroupConnections << connect(assetGroup, &RamAssetGroup::destroyed, this, &AssetGroupEditWidget::assetGroupDestroyed);

    this->setEnabled(Ramses::instance()->isProjectAdmin());
}

void AssetGroupEditWidget::update()
{
    if (!_assetGroup) return;

    this->setEnabled(false);

    //check if everything is alright
    if (!checkInput())
    {
        this->setEnabled(true);
        return;
    }

    _assetGroup->setName(nameEdit->text());
    _assetGroup->setShortName(shortNameEdit->text());

    _assetGroup->update();

    this->setEnabled(true);
}

void AssetGroupEditWidget::revert()
{
    setAssetGroup(_assetGroup);
}

bool AssetGroupEditWidget::checkInput()
{
    if (!_assetGroup) return false;

    if (shortNameEdit->text() == "")
    {
        statusLabel->setText("Short name cannot be empty!");
        updateButton->setEnabled(false);
        return false;
    }

    statusLabel->setText("");
    updateButton->setEnabled(true);
    return true;
}

void AssetGroupEditWidget::assetGroupDestroyed(QObject */*o*/)
{
    setAssetGroup(nullptr);
}

void AssetGroupEditWidget::dbiLog(DuQFLog m)
{
    if (m.type() != DuQFLog::Debug) statusLabel->setText(m.message());
}
