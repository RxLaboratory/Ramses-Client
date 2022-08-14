#ifndef TEMPLATEASSETGROUPEDITWIDGET_H
#define TEMPLATEASSETGROUPEDITWIDGET_H

#include "objecteditwidget.h"
#include "ramtemplateassetgroup.h"

class TemplateAssetGroupEditWidget : public ObjectEditWidget
{
    Q_OBJECT

public:
    explicit TemplateAssetGroupEditWidget(QWidget *parent = nullptr);
    explicit TemplateAssetGroupEditWidget(RamTemplateAssetGroup *templateAssetGroup, QWidget *parent = nullptr);

    RamTemplateAssetGroup *assetGroup() const;

protected:
    virtual void reInit(RamObject *o) override;

private slots:

private:
    void setupUi();

    RamTemplateAssetGroup *m_assetGroup;
};

#endif // TEMPLATEASSETGROUPEDITWIDGET_H
