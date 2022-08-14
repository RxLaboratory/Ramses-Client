#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include <QComboBox>

#include "processmanager.h"
#include "ramobjectlistcombobox.h"
#include "data-models/ramprojectfiltermodel.h"
#include "ramproject.h"

class ProjectSelectorWidget : public RamObjectListComboBox<RamProject*>
{
    Q_OBJECT
public:
    ProjectSelectorWidget(QWidget *parent = nullptr);
private slots:
    void setCurrentProject(RamObject *projObj);
    void currentProjectChanged(RamProject *p);
    void userChanged(RamUser *user);
private:
    RamProjectFilterModel *m_projectFilter;
    ProcessManager *m_pm;
};

#endif // PROJECTSELECTORWIDGET_H
