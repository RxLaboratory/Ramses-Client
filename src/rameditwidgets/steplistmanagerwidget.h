#ifndef STEPLISTMANAGERWIDGET_H
#define STEPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "stepeditwidget.h"

class StepListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    StepListManagerWidget(QWidget *parent = nullptr);

protected slots:
    void createObject() override;
private slots:
    void changeProject(RamProject *project);

    void templateStepInserted(const QModelIndex &parent, int first, int last);
    void newTemplate(RamObject *obj);
    void templateStepRemoved(const QModelIndex &parent, int first, int last);
    void templateStepChanged();

    void actionCreate();

private:
    QMenu *ui_createMenu;
};

#endif // STEPLISTMANAGERWIDGET_H
