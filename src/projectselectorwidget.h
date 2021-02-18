#ifndef PROJECTSELECTORWIDGET_H
#define PROJECTSELECTORWIDGET_H

#include <QComboBox>

#include "ramses.h"

class ProjectSelectorWidget : public QComboBox
{
    Q_OBJECT
public:
    ProjectSelectorWidget(QWidget *parent = nullptr);
private slots:
    void newProject(RamProject *project);
    void removeProject(QObject *o);
    void projectChanged();
    void setCurrentProject(int index);
};

#endif // PROJECTSELECTORWIDGET_H
