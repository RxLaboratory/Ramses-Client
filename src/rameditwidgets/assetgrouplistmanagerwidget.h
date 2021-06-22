#ifndef ASSETGROUPLISTMANAGERWIDGET_H
#define ASSETGROUPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "assetgroupeditwidget.h"

class AssetGroupListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetGroupListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() override;

private slots:
    void changeProject(RamProject *project);
};

#endif // ASSETGROUPLISTMANAGERWIDGET_H
