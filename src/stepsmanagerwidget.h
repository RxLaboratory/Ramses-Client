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
    void removeStep(QObject *step);
    void stepChanged();
    void newTemplateStep(RamStep *step);
    void removeTemplateStep(QObject *o);
    void templateStepChanged();

private:
    StepEditWidget *stepWidget;
    QMenu *assignMenu;
    QAction *actionCreateStep;
    QMetaObject::Connection _currentProjectConnection;
};

#endif // STEPSMANAGERWIDGET_H
