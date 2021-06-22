#ifndef ASSETEDITWIDGET_H
#define ASSETEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

/**
 * @brief The AssetEditWidget class is the Widget used to edit an Asset, including its Status History
 */
class AssetEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    AssetEditWidget(QWidget *parent = nullptr);
    AssetEditWidget(RamAsset *asset, QWidget *parent = nullptr);
    RamAsset *asset() const;

public slots:
    void setObject(RamObject *obj) override;

protected slots:
    void update() override;

private:
    RamAsset *_asset = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    QLineEdit *tagsEdit;
    RamObjectListComboBox *assetGroupBox;
};

#endif // ASSETEDITWIDGET_H
