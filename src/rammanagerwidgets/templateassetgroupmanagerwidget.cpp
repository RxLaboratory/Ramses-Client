#include "templateassetgroupmanagerwidget.h"

#include "ramses.h"
#include "duqf-widgets/duicon.h"

TemplateAssetGroupManagerWidget::TemplateAssetGroupManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->templateAssetGroups(),
        "Template asset groups",
        DuIcon(":icons/asset-group"),
        parent )
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    ui_listWidget->setSortable(true);
}

RamTemplateAssetGroup *TemplateAssetGroupManagerWidget::createObject()
{
    RamTemplateAssetGroup *ag = new RamTemplateAssetGroup(
                "NEW",
                "New Template group");
    //Ramses::instance()->templateAssetGroups()->append(ag);
    ag->edit();
    return ag;
}
