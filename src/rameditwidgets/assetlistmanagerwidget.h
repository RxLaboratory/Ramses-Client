#ifndef ASSETLISTMANAGERWIDGET_H
#define ASSETLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "asseteditwidget.h"

class AssetListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    AssetListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() override;

private slots:
    void changeProject(RamProject *project);
};

#endif // ASSETLISTMANAGERWIDGET_H
