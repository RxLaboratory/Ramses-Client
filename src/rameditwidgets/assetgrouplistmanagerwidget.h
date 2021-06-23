#ifndef ASSETGROUPLISTMANAGERWIDGET_H
#define ASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "assetgroupeditwidget.h"

class AssetGroupListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() override;

private slots:
    void changeProject(RamProject *project);

    void templateInserted(const QModelIndex &parent, int first, int last);
    void newTemplate(RamObject *obj);
    void templateRemoved(const QModelIndex &parent, int first, int last);
    void templateChanged();

    void actionCreate();

private:
    QMenu *ui_createMenu;
};

#endif // ASSETGROUPLISTMANAGERWIDGET_H
