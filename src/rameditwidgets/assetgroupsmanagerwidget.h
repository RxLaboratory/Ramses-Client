#ifndef ASSETGROUPSMANAGERWIDGET_H
#define ASSETGROUPSMANAGERWIDGET_H

#include <QMenu>

#include "listmanagerwidget.h"
#include "assetgroupeditwidget.h"

class AssetGroupsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    AssetGroupsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    void assignAssetGroup();
    void createAssetGroup();
    void newAssetGroup(RamAssetGroup *assetGroup);
    void assetGroupRemoved(QString uuid);
    void assetGroupRemoved(RamObject *o);
    void assetGroupChanged();
    void newTemplateAssetGroup(RamObject *assetGroup);
    void templateAssetGroupRemoved(RamObject *o);
    void templateAssetGroupChanged();

private:
    AssetGroupEditWidget *assetGroupWidget;
    QMenu *assignMenu;
    QAction *actionCreateAssetGroup;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // ASSETGROUPSMANAGERWIDGET_H