#ifndef PROJECTSMANAGERWIDGET_H
#define PROJECTSMANAGERWIDGET_H

#include "listmanagerwidget.h"
#include "projecteditwidget.h"

class ProjectsManagerWidget : public ListManagerWidget
{
    Q_OBJECT
public:
    ProjectsManagerWidget(QWidget *parent = nullptr);

protected slots:
    void currentDataChanged(QVariant data) Q_DECL_OVERRIDE;
    void createItem() Q_DECL_OVERRIDE;
    void removeItem(QVariant data) Q_DECL_OVERRIDE;

private slots:
    void newProject(RamProject *project);
    void projectRemoved(RamObject *project);
    void projectChanged();

private:
    ProjectEditWidget *projectWidget;
};

#endif // PROJECTSMANAGERWIDGET_H
