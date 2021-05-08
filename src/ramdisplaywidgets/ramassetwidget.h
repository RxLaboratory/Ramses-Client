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

private:
    RamAsset *_asset;
};

#endif // RAMASSETWIDGET_H