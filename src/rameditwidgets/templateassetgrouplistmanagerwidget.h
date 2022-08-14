#ifndef TEMPLATEASSETGROUPLISTMANAGERWIDGET_H
#define TEMPLATEASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramtemplateassetgroup.h"

class TemplateAssetGroupListManagerWidget : public ObjectListManagerWidget<RamTemplateAssetGroup*, int>
{
    Q_OBJECT
public:
    TemplateAssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamTemplateAssetGroup *createObject() override;
};

#endif // TEMPLATEASSETGROUPLISTMANAGERWIDGET_H
