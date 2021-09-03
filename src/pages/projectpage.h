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

    void assignUser(RamObject *userObj);
    void unAssignUser(RamObject *userObj);
    void userAssigned(const QModelIndex &parent,int first,int last);
    void userUnassigned(const QModelIndex &parent,int first,int last);

    void createStepFromTemplate(RamObject *stepObj);
    void createAssetGroupFromTemplate(RamObject *agObj);

    void createShots();

private:
    ProjectEditWidget *ui_currentProjectSettings;

    QList<QMetaObject::Connection> m_userConnections;

    RamObjectListMenu *ui_assignUserMenu;
    RamObjectListMenu *ui_unAssignUserMenu;
};

#endif // PROJECTPAGE_H
