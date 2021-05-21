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

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void moveAsset();
    void newAssetGroup(RamObject *ag);
    void assetGroupChanged(RamObject *o);
    void assetGroupRemoved(RamObject *o);

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
