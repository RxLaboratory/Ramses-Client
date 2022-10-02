#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include <QComboBox>

#include "processmanager.h"
#include "ramobjectcombobox.h"
#include "ramobjectsortfilterproxymodel.h"
#include "ramproject.h"

class ProjectSelectorWidget : public RamObjectComboBox
{
    Q_OBJECT
public:
    ProjectSelectorWidget(QWidget *parent = nullptr);
private slots:
    void setCurrentProject(RamObject *projObj);
    void currentProjectChanged(RamProject *p);
    void userChanged(RamUser *user);
private:
    RamObjectSortFilterProxyModel *m_projectFilter;
    ProcessManager *m_pm;
};

#endif // PROJECTSELECTORWIDGET_H
