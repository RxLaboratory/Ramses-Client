#ifndef STEPLISTMANAGERWIDGET_H
#define STEPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "data-views/ramobjectlistmenu.h"

class StepListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    StepListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamStep *createObject() override;
private slots:
    void changeProject(RamProject *project);

    void createFromTemplate(RamObject *templateStepObj);

private:
    RamObjectListMenu *ui_createMenu;
};

#endif // STEPLISTMANAGERWIDGET_H
