#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include "duqf-widgets/settingswidget.h"
#include "ramses.h"
#include "projecteditwidget.h"
#include "steplistmanagerwidget.h"
#include "pipefilelistmanagerwidget.h"
#include "assetgrouplistmanagerwidget.h"
#include "assetlistmanagerwidget.h"
#include "sequencelistmanagerwidget.h"
#include "shotlistmanagerwidget.h"

class ProjectPage : public SettingsWidget
{
    Q_OBJECT
public:
    ProjectPage(QWidget *parent = nullptr);
private slots:
    void currentProjectChanged(RamProject *project);

private:
    ProjectEditWidget *ui_currentProjectSettings;
};

#endif // PROJECTPAGE_H
