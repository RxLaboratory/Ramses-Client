#ifndef TEMPLATESTEPSMANAGERWIDGET_H
#define TEMPLATESTEPSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "stepeditwidget.h"

class TemplateStepsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    TemplateStepsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newStep(RamStep *step);
    void removeStep(QObject *step);
    void stepChanged();

private:
    StepEditWidget *stepWidget;
};

#endif // TEMPLATESTEPSMANAGERWIDGET_H
