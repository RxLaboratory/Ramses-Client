#ifndef TEMPLATEASSETGROUPEDITWIDGET_H
#define TEMPLATEASSETGROUPEDITWIDGET_H

#include "ui_templateassetgroupeditwidget.h"
#include "ramses.h"

class TemplateAssetGroupEditWidget : public QWidget, private Ui::TemplateAssetGroupEditWidget
{
    Q_OBJECT

public:
    explicit TemplateAssetGroupEditWidget(QWidget *parent = nullptr);

    RamAssetGroup *assetGroup() const;
    void setAssetGroup(RamAssetGroup *assetGroup);

private slots:
    void update();
    void revert();
    bool checkInput();
    void assetGroupRemoved(RamObject *o);
    void dbiLog(DuQFLog m);

private:
    RamAssetGroup *_assetGroup;
    QMetaObject::Connection _currentAssetGroupConnection;
};

#endif // TEMPLATEASSETGROUPEDITWIDGET_H
