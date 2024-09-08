#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include <QWizard>
#include <QDateEdit>
#include <QDoubleSpinBox>

#include "duwidgets/dulisteditview.h"
#include "frameratewidget.h"
#include "ramjsonobjectmodel.h"
#include "ramproject.h"
#include "ramuser.h"
#include "resolutionwidget.h"
#include "wizards/loginwizardpage.h"
#include "wizards/ramobjectpropertieswizardpage.h"
#include "wizards/ramdatabasepathswizardpage.h"
#include "wizards/ramuserswizardpage.h"

class ProjectWizard : public QWizard
{
    Q_OBJECT
public:
    enum PageId {
        LoginPage,
        DetailsPage,
        SettingsPage,
        UsersPage,
        PipelinePage,
        PathsPage
    };

    ProjectWizard(bool team = false, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    virtual void done(int r) override;

private slots:
    void editStep(const QModelIndex &index);
    void finishProjectSetup();

private:
    void setupUi();
    void connectEvents();

    QWizardPage *createProjectSettingsPage();
    QWizardPage *createPipelinePage();

    RamObjectPropertiesWizardPage *ui_detailsPage;
    RamDatabasePathsWizardPage *ui_pathsPage;
    LoginWizardPage *ui_loginPage;
    RamUsersWizardPage *ui_usersPage;

    ResolutionWidget *ui_resolutionWidget;
    FramerateWidget *ui_framerateWidget;
    QDateEdit *ui_deadlineEdit;
    QDoubleSpinBox *ui_parBox;
    DuListEditView *ui_stepList;

    RamJsonObjectModel *_steps;
    RamJsonObjectModel *_users;

    bool _isTeamProject;
    QString _projectUuid;
    QString _localUserUuid;

    // Utils
    bool askRemoveExistingFile(const QString &dbPath );
    bool createDatabase(const QString &dbPath);
    QString createServerData();
    bool checkServerReply(const QJsonObject &reply);
    RamUser *createLocalUser();
    RamProject *createLocalProject();
};

#endif // PROJECTWIZARD_H
