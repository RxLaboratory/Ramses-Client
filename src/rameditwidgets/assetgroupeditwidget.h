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
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private slots:
    void createAsset();

private:
    RamAssetGroup *m_assetGroup;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *ui_folderWidget;
    ObjectListEditWidget *ui_assetsList;
};

#endif // ASSETGROUPEDITWIDGET_H
