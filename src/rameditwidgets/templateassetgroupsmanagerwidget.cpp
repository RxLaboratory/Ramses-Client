#include "templateassetgroupsmanagerwidget.h"

TemplateAssetGroupsManagerWidget::TemplateAssetGroupsManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    assetGroupWidget = new TemplateAssetGroupEditWidget(this);
    this->setWidget(assetGroupWidget);
    assetGroupWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    foreach(RamAssetGroup *ag, Ramses::instance()->templateAssetGroups()) newAssetGroup(ag);

    connect(Ramses::instance(), &Ramses::newTemplateAssetGroup, this, &TemplateAssetGroupsManagerWidget::newAssetGroup);
}

void TemplateAssetGroupsManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamAssetGroup *ag, Ramses::instance()->templateAssetGroups())
    {
        if (ag->uuid() == data.toString())
        {
            assetGroupWidget->setAssetGroup(ag);
            assetGroupWidget->setEnabled(true);
            return;
        }
    }
    assetGroupWidget->setEnabled(false);
}

void TemplateAssetGroupsManagerWidget::createItem()
{
    Ramses::instance()->createTemplateAssetGroup();
}

void TemplateAssetGroupsManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeTemplateAssetGroup(data.toString());
}

void TemplateAssetGroupsManagerWidget::newAssetGroup(RamAssetGroup *ag)
{
    if (ag->uuid() != "")
    {
        QListWidgetItem *agItem = new QListWidgetItem(ag->name());
        agItem->setData(Qt::UserRole, ag->uuid());
        this->addItem(agItem);
        connect(ag, &RamStep::removed, this, &TemplateAssetGroupsManagerWidget::assetGroupRemoved);
        connect(ag, &RamStep::changed, this, &TemplateAssetGroupsManagerWidget::assetGroupChanged);
    }
}

void TemplateAssetGroupsManagerWidget::assetGroupRemoved(RamObject *assetGroup)
{
    removeData(assetGroup->uuid());
}

void TemplateAssetGroupsManagerWidget::assetGroupChanged()
{
    RamAssetGroup *ag = (RamAssetGroup*)QObject::sender();
    updateItem(ag->uuid(), ag->name());
}
