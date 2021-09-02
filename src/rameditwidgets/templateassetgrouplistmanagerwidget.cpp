#include "templateassetgrouplistmanagerwidget.h"

TemplateAssetGroupListManagerWidget::TemplateAssetGroupListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateAssetGroups(),
        new TemplateAssetGroupEditWidget(),
        "Template asset groups",
        QIcon(":icons/asset-group"),
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

RamObject *TemplateAssetGroupListManagerWidget::createObject()
{
    RamAssetGroup *ag = new RamAssetGroup(
                "NEW",
                "New Template group");
    Ramses::instance()->templateAssetGroups()->append(ag);
    editObject(ag);
    return ag;
}
