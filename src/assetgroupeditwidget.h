#ifndef ASSETGROUPEDITWIDGET_H
#define ASSETGROUPEDITWIDGET_H

#include "ui_assetgroupeditwidget.h"
#include "ramses.h"

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
    void assetGroupDestroyed(QObject *o);
    void dbiLog(DuQFLog m);

private:
    RamAssetGroup *_assetGroup;
    QList<QMetaObject::Connection> _assetGroupConnections;
};

#endif // ASSETGROUPEDITWIDGET_H
