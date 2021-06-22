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
};

#endif // STEPLISTMANAGERWIDGET_H
