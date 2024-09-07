#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include "duwidgets/ducombobox.h"
#include "progressmanager.h"
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
    void ramsesReady();
private:
    RamObjectSortFilterProxyModel *m_projectFilter;
    ProgressManager *m_pm;
};

#endif // PROJECTSELECTORWIDGET_H
