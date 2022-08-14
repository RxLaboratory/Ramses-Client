#ifndef PROJECTPAGE_H
#define PROJECTPAGE_H

#include "duqf-widgets/settingswidget.h"
#include "projecteditwidget.h"
#include "ramtemplateassetgroup.h"

class ProjectPage : public SettingsWidget
{
    Q_OBJECT
public:
    ProjectPage(QWidget *parent = nullptr);
private slots:
    void currentProjectChanged(RamProject *project);

    void assignUser(RamUser *user);
    void unAssignUser(RamUser *user);
    void userAssigned(const QModelIndex &parent,int first,int last);
    void userUnassigned(const QModelIndex &parent,int first,int last);

    void createStepFromTemplate(RamTemplateStep *templateStep);
    void createAssetGroupFromTemplate(RamTemplateAssetGroup *templateAG);

    void createShots();

private:
    ProjectEditWidget *ui_currentProjectSettings;

    QList<QMetaObject::Connection> m_userConnections;

    RamObjectListMenu *ui_assignUserMenu;
    RamObjectListMenu *ui_unAssignUserMenu;
};

#endif // PROJECTPAGE_H
