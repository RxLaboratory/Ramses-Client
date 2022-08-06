#ifndef ASSETGROUPLISTMANAGERWIDGET_H
#define ASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "data-views/ramobjectlistmenu.h"
#include "ramtemplateassetgroup.h"

class AssetGroupListManagerWidget : public ObjectListManagerWidget<RamAssetGroup*, RamProject*>
{
    Q_OBJECT
public:
    AssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamAssetGroup *createObject() override;

private slots:
    void changeProject(RamProject *project);

    void createFromTemplate(RamTemplateAssetGroup *templateAG);

private:
    RamObjectListMenu<RamTemplateAssetGroup *> *ui_createMenu;
};

#endif // ASSETGROUPLISTMANAGERWIDGET_H
