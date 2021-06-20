#ifndef ASSETGROUPEDITWIDGET_H
#define ASSETGROUPEDITWIDGET_H

#include "objecteditwidget.h"
#include "objectlisteditwidget.h"
#include "ramassetgroup.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"
#include "ramses.h"

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

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void createAsset();

private:
    RamAssetGroup *_assetGroup;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    ObjectListEditWidget *assetsList;

    bool _creatingAsset = false;
};

#endif // ASSETGROUPEDITWIDGET_H
