#ifndef ASSETSMANAGERWIDGET_H
#define ASSETSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "asseteditwidget.h"

class AssetsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    AssetsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    void newAsset(RamAsset *asset);
    void assetChanged();
    void assetRemoved(QString uuid);
    void filter(QString assetGroupUuid);
    void addAssets(RamAssetGroup *assetGroup);
    void assetGroupChanged();
    void assetGroupRemoved(QString uuid);
    void newAssetGroup(RamAssetGroup *assetGroup);

private:
    AssetEditWidget *assetWidget;
    QList<QMetaObject::Connection> _projectConnections;
    QList<QMetaObject::Connection> _assetGroupsConnections;
};

#endif // ASSETSMANAGERWIDGET_H
