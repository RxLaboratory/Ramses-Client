#ifndef STEPSMANAGERWIDGET_H
#define STEPSMANAGERWIDGET_H

#include <QMenu>

#include "listmanagerwidget.h"
#include "stepeditwidget.h"

class StepsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    StepsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void changeProject(RamProject *project);
    void assignStep();
    void createStep();
    void newStep(RamStep *step);
    void stepRemoved(QString uuid);
    void stepRemoved(RamObject *step);
    void stepChanged();
    void newTemplateStep(RamStep *step);
    void templateStepRemoved(RamObject *o);
    void templateStepChanged();
    void moveStepUp();
    void moveStepDown();
    void updateStepsOrder();

private:
    StepEditWidget *stepWidget;
    QMenu *assignMenu;
    QToolButton *downButton;
    QToolButton *upButton;
    QAction *actionCreateStep;
    QList<QMetaObject::Connection> _projectConnections;
};

#endif // STEPSMANAGERWIDGET_H
