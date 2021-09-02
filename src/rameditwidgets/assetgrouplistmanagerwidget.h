#ifndef ASSETGROUPLISTMANAGERWIDGET_H
#define ASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "assetgroupeditwidget.h"
#include "data-views/ramobjectlistmenu.h"

class AssetGroupListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamObject *createObject() override;

private slots:
    void changeProject(RamProject *project);

    void createFromTemplate(RamObject *obj);

private:
    RamObjectListMenu *ui_createMenu;
};

#endif // ASSETGROUPLISTMANAGERWIDGET_H
