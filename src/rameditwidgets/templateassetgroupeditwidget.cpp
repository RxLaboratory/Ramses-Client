#include "templateassetgroupeditwidget.h"

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
}

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(RamTemplateAssetGroup *templateAssetGroup, QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    setObject(templateAssetGroup);
}

RamTemplateAssetGroup *TemplateAssetGroupEditWidget::assetGroup() const
{
    return m_assetGroup;
}

void TemplateAssetGroupEditWidget::reInit(RamObject *obj)
{
    m_assetGroup = qobject_cast<RamTemplateAssetGroup*>(obj);
}

void TemplateAssetGroupEditWidget::setupUi()
{
    ui_mainLayout->addStretch();
}
