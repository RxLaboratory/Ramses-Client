#ifndef PROJECTLISTMANAGERWIDGET_H
#define PROJECTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramses.h"
#include "projecteditwidget.h"

class ProjectListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ProjectListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamObject *createObject() override;
};

#endif // PROJECTLISTMANAGERWIDGET_H
