#ifndef PROJECTLISTMANAGERWIDGET_H
#define PROJECTLISTMANAGERWIDGET_H

#include "objectlistmanagerwidget.h"
#include "ramproject.h"

class ProjectListManagerWidget : public ObjectListManagerWidget
{
    Q_OBJECT
public:
    ProjectListManagerWidget(QWidget *parent = nullptr);

protected slots:
    RamProject *createObject() override;
};

#endif // PROJECTLISTMANAGERWIDGET_H
