#ifndef ASSETEDITWIDGET_H
#define ASSETEDITWIDGET_H

#include "ui_asseteditwidget.h"
#include "ramses.h"

class AssetEditWidget : public QWidget, private Ui::AssetEditWidget
{
    Q_OBJECT

public:
    explicit AssetEditWidget(QWidget *parent = nullptr);

    RamAsset *asset() const;
    void setAsset(RamAsset *asset);

    void setProject(RamProject *project);

private slots:
    void update();
    void revert();
    bool checkInput();
    void assetDestroyed(QObject *o);
    void newAssetGroup(RamAssetGroup *ag);
    void assetGroupChanged();
    void assetGroupRemoved(QString uuid);
    void dbiLog(DuQFLog m);

private:
    RamAsset *_asset;
    RamProject *_project;
    QList<QMetaObject::Connection> _assetConnections;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // ASSETEDITWIDGET_H
