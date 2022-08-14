#ifndef ASSETGROUPLISTMANAGERWIDGET_H
#define ASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "data-views/ramobjectlistmenu.h"
#include "ramtemplateassetgroup.h"
#include "ramassetgroup.h"

class AssetGroupListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamAssetGroup *createObject() override;

private slots:
    void changeProject(RamProject *project);

    void createFromTemplate(RamObject *templateAGObj);

private:
    RamObjectListMenu *ui_createMenu;
};

#endif // ASSETGROUPLISTMANAGERWIDGET_H
