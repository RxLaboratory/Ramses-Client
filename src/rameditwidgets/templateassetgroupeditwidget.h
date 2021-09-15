#ifndef TEMPLATEASSETGROUPEDITWIDGET_H
#define TEMPLATEASSETGROUPEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramses.h"

class TemplateAssetGroupEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit TemplateAssetGroupEditWidget(QWidget *parent = nullptr);
    explicit TemplateAssetGroupEditWidget(RamAssetGroup *templateAssetGroup, QWidget *parent = nullptr);

    RamAssetGroup *assetGroup() const;

public slots:
    void setObject(RamObject *obj) override;

private slots:

private:
    void setupUi();
    RamAssetGroup *_assetGroup;
};

#endif // TEMPLATEASSETGROUPEDITWIDGET_H
