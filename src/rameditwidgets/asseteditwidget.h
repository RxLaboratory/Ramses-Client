#ifndef ASSETEDITWIDGET_H
#define ASSETEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"
#include "statushistorywidget.h"
#include "ramobjectlistcombobox.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

/**
 * @brief The AssetEditWidget class is the Widget used to edit an Asset, including its Status History
 */
class AssetEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit AssetEditWidget(QWidget *parent = nullptr);
    RamAsset *asset() const;

public slots:
    void setObject(RamObject *obj) Q_DECL_OVERRIDE;

protected slots:
    void update() Q_DECL_OVERRIDE;

private:
    RamAsset *_asset = nullptr;

    void setupUi();
    void connectEvents();

    DuQFFolderDisplayWidget *folderWidget;
    QLineEdit *tagsEdit;
    RamObjectListComboBox *assetGroupBox;
};

#endif // ASSETEDITWIDGET_H
