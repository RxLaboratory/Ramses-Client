#ifndef TEMPLATEASSETGROUPSMANAGERWIDGET_H
#define TEMPLATEASSETGROUPSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "templateassetgroupeditwidget.h"

class TemplateAssetGroupsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    TemplateAssetGroupsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newAssetGroup(RamAssetGroup *ag);
    void assetGroupRemoved(RamObject *assetGroup);
    void assetGroupChanged();

private:
    TemplateAssetGroupEditWidget *assetGroupWidget;
};

#endif // TEMPLATEASSETGROUPSMANAGERWIDGET_H
