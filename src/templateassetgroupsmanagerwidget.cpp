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
        connect(ag, &RamStep::destroyed, this, &TemplateAssetGroupsManagerWidget::removeAssetGroup);
        connect(ag, &RamStep::changed, this, &TemplateAssetGroupsManagerWidget::assetGroupChanged);
    }
}

void TemplateAssetGroupsManagerWidget::removeAssetGroup(QObject *assetGroup)
{
    RamAssetGroup *ag = (RamAssetGroup*)assetGroup;

    removeData(ag->uuid());
}

void TemplateAssetGroupsManagerWidget::assetGroupChanged()
{
    RamAssetGroup *ag = (RamAssetGroup*)QObject::sender();
    updateItem(ag->uuid(), ag->name());
}
