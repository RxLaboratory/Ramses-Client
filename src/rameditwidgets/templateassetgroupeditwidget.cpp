#include "templateassetgroupeditwidget.h"

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setupUi();
    setObject(nullptr);

    monitorDbQuery("updateTemplateAssetGroup");
}

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(RamAssetGroup *templateAssetGroup, QWidget *parent) :
    ObjectEditWidget(templateAssetGroup, parent)
{
    setupUi();
    setObject(templateAssetGroup);

    monitorDbQuery("updateTemplateAssetGroup");
}

RamAssetGroup *TemplateAssetGroupEditWidget::assetGroup() const
{
    return _assetGroup;
}

void TemplateAssetGroupEditWidget::setObject(RamObject *obj)
{
    RamAssetGroup *assetGroup = (RamAssetGroup*)obj;

    this->setEnabled(false);

    ObjectEditWidget::setObject(assetGroup);
    _assetGroup = assetGroup;

    if (!assetGroup) return;

    this->setEnabled(Ramses::instance()->isAdmin());

}

void TemplateAssetGroupEditWidget::setupUi()
{
    ui_mainLayout->addStretch();
}
