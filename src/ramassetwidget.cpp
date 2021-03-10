#include "ramassetwidget.h"

RamAssetWidget::RamAssetWidget(RamAsset *asset, QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    _asset = asset;

    _editDialog = new QDialog(this);
    _editDialog->setModal(true);
    _editDialog->setWindowFlag(Qt::FramelessWindowHint, true);
    QVBoxLayout *l = new QVBoxLayout(_editDialog);
    l->setContentsMargins(3,3,3,3);
    l->setSpacing(3);
    AssetEditWidget *aw = new AssetEditWidget(_editDialog);
    aw->setAsset(_asset);
    l->addWidget(aw);
    _editDialog->setLayout(l);

    assetChanged();
    userChanged();

    connect(asset, &RamAsset::changed, this, &RamAssetWidget::assetChanged);
    connect(asset, &RamAsset::removed, this, &RamAssetWidget::deleteLater);
    connect(Ramses::instance(), &Ramses::loggedIn, this, &RamAssetWidget::userChanged);
    connect(editButton, SIGNAL(clicked()), this, SLOT(edit()));
    connect(aw, SIGNAL(accepted()), _editDialog, SLOT(accept()));
    connect(aw, SIGNAL(rejected()), _editDialog, SLOT(reject()));
}

void RamAssetWidget::assetChanged()
{
    nameLabel->setText(_asset->name());
}

void RamAssetWidget::userChanged()
{
    editButton->setVisible( Ramses::instance()->isLead() );
}

void RamAssetWidget::edit()
{
    _editDialog->exec();
}

RamAsset *RamAssetWidget::asset() const
{
    return _asset;
}
