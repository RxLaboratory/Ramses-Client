#include "ramassetwidget.h"

RamAssetWidget::RamAssetWidget(RamAsset *asset, QWidget *parent) :
    RamObjectWidget(asset, parent)
{
    _asset = asset;

    AssetEditWidget *aw = new AssetEditWidget(this);
    aw->setAsset(asset);
    setEditWidget(aw);

    setUserEditRole(RamUser::Lead);

    setIcon(":/icons/asset");

    completeUi();
    connectEvents();

    assetChanged();
}

RamAsset *RamAssetWidget::asset() const
{
    return _asset;
}

void RamAssetWidget::exploreClicked()
{
    explore(Ramses::instance()->path(_asset));
}

void RamAssetWidget::assetChanged()
{
    m_groupLabel->setText( _asset->assetGroup()->name() + " | " + _asset->shortName() );
    m_tagsLabel->setText(_asset->tags().join(", "));
}

void RamAssetWidget::completeUi()
{
    QString detailsStyle = "color: " + DuUI::getColor("medium-grey").name() + ";";
    detailsStyle += "font-style: italic; font-weight: 300;";
    detailsStyle += "padding-right:" + QString::number(DuUI::getSize("padding","medium")) + "px;";

    m_groupLabel = new QLabel();
    m_groupLabel->setStyleSheet(detailsStyle);
    primaryContentLayout->addWidget(m_groupLabel);

    m_tagsLabel = new QLabel();
    m_tagsLabel->setStyleSheet(detailsStyle);
    primaryContentLayout->addWidget(m_tagsLabel);

    setPrimaryContentHeight(50);
    showExploreButton(true);
}

void RamAssetWidget::connectEvents()
{
    connect(_asset, &RamAsset::changed, this, &RamAssetWidget::assetChanged);
}
