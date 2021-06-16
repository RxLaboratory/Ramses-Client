#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include <QComboBox>

#include "ramses.h"
#include "ramobjectlistcombobox.h"

class ProjectSelectorWidget : public RamObjectListComboBox
{
    Q_OBJECT
public:
    ProjectSelectorWidget(QWidget *parent = nullptr);
private slots:
    void setCurrentProject(int index);
    void currentProjectChanged(RamProject *p);
};

#endif // PROJECTSELECTORWIDGET_H
