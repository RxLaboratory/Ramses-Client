#ifndef APPLICATIONLISTMANAGERWIDGET_H
#define APPLICATIONLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramapplication.h"

class ApplicationListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ApplicationListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamApplication *createObject() override;
};

#endif // APPLICATIONLISTMANAGERWIDGET_H
