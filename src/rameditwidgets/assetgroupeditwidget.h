#ifndef ASSETGROUPEDITWIDGET_H
#define ASSETGROUPEDITWIDGET_H

#include "objecteditwidget.h"
#include "simpleobjectlist.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

/**
 * @brief The AssetGroupEditWidget class is used to edit AssetGroups and can be shown either in the main UI or in the Dock
 */
class AssetGroupEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit AssetGroupEditWidget(QWidget *parent = nullptr);
    explicit AssetGroupEditWidget(RamAssetGroup *ag, QWidget *parent = nullptr);

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void assetGroupChanged(RamObject *o);
    void newAsset(RamAsset *asset);
    void assetRemoved(RamAsset *asset);
    void addAsset();
    void removeAsset(RamObject *o);

private:
    RamAssetGroup *_assetGroup;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    SimpleObjectList *assetsList;

    bool _creatingAsset = false;
};

#endif // ASSETGROUPEDITWIDGET_H
