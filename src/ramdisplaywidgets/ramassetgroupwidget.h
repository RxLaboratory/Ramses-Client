#ifndef RAMASSETGROUPWIDGET_H
#define RAMASSETGROUPWIDGET_H

#include "ramobjectwidget.h"
#include "assetgroupeditwidget.h"

class RamAssetGroupWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamAssetGroupWidget(RamAssetGroup *assetGroup, QWidget *parent = nullptr);
    RamAssetGroup *assetGroup() const;

protected slots:
    void exploreClicked() Q_DECL_OVERRIDE;

private:
    RamAssetGroup *m_assetGroup;

    void completeUi();
};

#endif // RAMASSETGROUPWIDGET_H
