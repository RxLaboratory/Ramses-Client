#ifndef JOINTEAMPROJECTWIZARD_H
#define JOINTEAMPROJECTWIZARD_H

#include <QWizard>

#include "wizards/loginwizardpage.h"
#include "wizards/projectlistwizardpage.h"
#include "wizards/ramdatabasepathswizardpage.h"

class JoinTeamProjectWizard: public QWizard
{
    Q_OBJECT
public:
    enum PageId {
        LoginPage,
        ProjectPage,
        PathsPage
    };

    JoinTeamProjectWizard(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    virtual void done(int r) override;

private slots:
    void finishProjectSetup();

private:
    void setupUi();
    void connectEvents();

    RamDatabasePathsWizardPage *ui_pathsPage;
    LoginWizardPage *ui_loginPage;
    ProjectListWizardPage *ui_projectsPage;

    // Utils
    bool askRemoveExistingFile(const QString &dbPath );
    bool createDatabase(const QString &dbPath);
};

#endif // JOINTEAMPROJECTWIZARD_H
