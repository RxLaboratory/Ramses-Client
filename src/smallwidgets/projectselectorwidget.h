#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include <QComboBox>

#include "ramses.h"
#include "processmanager.h"
#include "ramobjectlistcombobox.h"
#include "data-models/ramprojectfiltermodel.h"

class ProjectSelectorWidget : public RamObjectListComboBox
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
