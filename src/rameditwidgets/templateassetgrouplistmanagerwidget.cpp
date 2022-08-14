#include "templateassetgrouplistmanagerwidget.h"

#include "ramses.h"

TemplateAssetGroupListManagerWidget::TemplateAssetGroupListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget<RamTemplateAssetGroup*, int>(
        Ramses::instance()->templateAssetGroups(),
        "Template asset groups",
        QIcon(":icons/asset-group"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamTemplateAssetGroup*, int>::RemoveObjects);
}

RamTemplateAssetGroup *TemplateAssetGroupListManagerWidget::createObject()
{
    RamTemplateAssetGroup *ag = new RamTemplateAssetGroup(
                "NEW",
                "New Template group");
    Ramses::instance()->templateAssetGroups()->append(ag);
    ag->edit();
    return ag;
}
