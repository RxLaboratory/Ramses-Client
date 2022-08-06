#ifndef APPLICATIONLISTMANAGERWIDGET_H
#define APPLICATIONLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"

class ApplicationListManagerWidget : public ObjectListManagerWidget<RamApplication*, int>
{
    Q_OBJECT
public:
    ApplicationListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamApplication *createObject() override;
};

#endif // APPLICATIONLISTMANAGERWIDGET_H
