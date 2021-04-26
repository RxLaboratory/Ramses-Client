#ifndef ASSETEDITWIDGET_H
#define ASSETEDITWIDGET_H

#include <QComboBox>

#include "objecteditwidget.h"
#include "ramses.h"
#include "statushistorywidget.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

/**
 * @brief The AssetEditWidget class is the Widget used to edit an Asset, including its Status History
 */
class AssetEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit AssetEditWidget(RamAsset *asset = nullptr, QWidget *parent = nullptr);

    RamAsset *asset() const;
    void setAsset(RamAsset *asset);

protected slots:
    void update() Q_DECL_OVERRIDE;
    bool checkInput() Q_DECL_OVERRIDE;

private slots:
    void moveAsset();
    void assetChanged(RamObject *o);
    void newAssetGroup(RamAssetGroup *ag);
    void assetGroupChanged(RamObject *o);
    void assetGroupRemoved(QString uuid);

private:
    RamAsset *_asset;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    QLineEdit *tagsEdit;
    QComboBox *assetGroupBox;
    StatusHistoryWidget *statusHistoryWidget;

    RamAssetGroup *assetGroup();
    RamProject *project();
};

#endif // ASSETEDITWIDGET_H
