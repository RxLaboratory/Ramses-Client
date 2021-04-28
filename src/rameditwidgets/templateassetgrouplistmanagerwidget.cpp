#include "templateassetgrouplistmanagerwidget.h"

TemplateAssetGroupListManagerWidget::TemplateAssetGroupListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->templateAssetGroups(),
        new TemplateAssetGroupEditWidget(),
        "Template asset groups",
        parent )
{

}

void TemplateAssetGroupListManagerWidget::createObject()
{
    Ramses::instance()->createTemplateAssetGroup();
}
