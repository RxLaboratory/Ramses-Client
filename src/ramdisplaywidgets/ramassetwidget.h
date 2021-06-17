#ifndef RAMASSETWIDGET_H
#define RAMASSETWIDGET_H

#include <QDockWidget>

#include "ramobjectwidget.h"
#include "ramses.h"
#include "asseteditwidget.h"

/**
 * @brief The RamAssetWidget class is a small widget used to show an Asset anywhere in the UI
 */
class RamAssetWidget : public RamObjectWidget
{
    Q_OBJECT

public:
    explicit RamAssetWidget(RamAsset *asset, QWidget *parent = nullptr);
    RamAsset *asset() const;

protected slots:
    void exploreClicked() Q_DECL_OVERRIDE;

private slots:
    void assetChanged();

private:
    RamAsset *_asset;

    void completeUi();
    void connectEvents();

    QLabel *m_tagsLabel;
    QLabel *m_groupLabel;
};

#endif // RAMASSETWIDGET_H
