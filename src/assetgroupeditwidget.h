#ifndef ASSETGROUPEDITWIDGET_H
#define ASSETGROUPEDITWIDGET_H

#include "ui_assetgroupeditwidget.h"
#include "ramassetwidget.h"
#include "duqf-widgets/duqffolderdisplaywidget.h"

class AssetGroupEditWidget : public QWidget, private Ui::AssetGroupEditWidget
{
    Q_OBJECT

public:
    explicit AssetGroupEditWidget(QWidget *parent = nullptr);

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

private slots:
    void update();
    void revert();
    bool checkInput();
    void assetGroupRemoved(RamObject *o);
    void addAsset();
    void removeAsset();
    void newAsset(RamAsset *a);
    void assetRemoved(QString uuid);
    void dbiLog(DuQFLog m);

private:
    DuQFFolderDisplayWidget *folderWidget;
    RamAssetGroup *_assetGroup;
    QList<QMetaObject::Connection> _assetGroupConnections;
};

#endif // ASSETGROUPEDITWIDGET_H
