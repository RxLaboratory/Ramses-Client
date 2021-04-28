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
private:
    RamAssetGroup *m_assetGroup;
};

#endif // RAMASSETGROUPWIDGET_H
