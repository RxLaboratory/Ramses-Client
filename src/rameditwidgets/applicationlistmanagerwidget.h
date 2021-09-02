#ifndef APPLICATIONLISTMANAGERWIDGET_H
#define APPLICATIONLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "applicationeditwidget.h"

class ApplicationListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ApplicationListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamObject *createObject() override;
};

#endif // APPLICATIONLISTMANAGERWIDGET_H
