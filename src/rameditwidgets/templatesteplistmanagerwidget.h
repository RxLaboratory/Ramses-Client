#ifndef TEMPLATESTEPLISTMANAGERWIDGET_H
#define TEMPLATESTEPLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "templatestepeditwidget.h"

class TemplateStepListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    TemplateStepListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamObject *createObject() override;
};

#endif // TEMPLATESTEPLISTMANAGERWIDGET_H
