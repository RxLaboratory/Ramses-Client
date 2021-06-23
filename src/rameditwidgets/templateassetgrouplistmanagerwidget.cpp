#include "templateassetgrouplistmanagerwidget.h"

TemplateAssetGroupListManagerWidget::TemplateAssetGroupListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateAssetGroups(),
        new TemplateAssetGroupEditWidget(),
        "Template asset groups",
        parent )
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
}

void TemplateAssetGroupListManagerWidget::createObject()
{
    RamAssetGroup *ag = new RamAssetGroup(
                "NEW",
                "New Template group");
    Ramses::instance()->templateAssetGroups()->append(ag);
    editObject(ag);
}
