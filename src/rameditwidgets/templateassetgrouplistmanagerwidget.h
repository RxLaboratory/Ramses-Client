#ifndef TEMPLATEASSETGROUPLISTMANAGERWIDGET_H
#define TEMPLATEASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "templateassetgroupeditwidget.h"

class TemplateAssetGroupListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    TemplateAssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() override;
};

#endif // TEMPLATEASSETGROUPLISTMANAGERWIDGET_H
