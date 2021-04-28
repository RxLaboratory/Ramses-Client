#include "templateassetgroupeditwidget.h"

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(QWidget *parent) :
    ObjectEditWidget(parent)
{
    setObject(nullptr);
}

TemplateAssetGroupEditWidget::TemplateAssetGroupEditWidget(RamAssetGroup *templateAssetGroup, QWidget *parent) :
    ObjectEditWidget(templateAssetGroup, parent)
{
    setObject(templateAssetGroup);
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

    _objectConnections << connect(assetGroup, &RamAssetGroup::changed, this, &TemplateAssetGroupEditWidget::assetGroupChanged);
}

void TemplateAssetGroupEditWidget::assetGroupChanged(RamObject *o)
{
    if (updating) return;
    Q_UNUSED(o);
    setObject(_assetGroup);
}
