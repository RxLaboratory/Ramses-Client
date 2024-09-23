#ifndef SERVERWIZARD_H
#define SERVERWIZARD_H

#include "wizards/loginwizardpage.h"
#include "wizards/ramprojectswizardpage.h"
#include "wizards/ramuserswizardpage.h"
#include <QWizard>

class ServerWizard : public QWizard
{
    Q_OBJECT
public:
    enum PageId {
        LoginPage,
        UsersPage,
        ProjectsPage,
    };

    ServerWizard(QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());


protected:
    virtual void done(int r) override;

private:
    void setupUi();
    void connectEvents();

    void pageChanged(int id);
    void removeUsers(const QModelIndex &parent, int first, int last);
    void removeProjects(const QModelIndex &parent, int first, int last);
    void applyChanges();
    void cancelChanges();

    LoginWizardPage *ui_loginPage;
    RamUsersWizardPage *ui_usersPage;
    RamProjectsWizardPage *ui_projectsPage;

    RamJsonObjectModel *_users;
    RamJsonObjectModel *_projects;

    QVector<QJsonObject> _removedUsers;
    QVector<QJsonObject> _removedProjects;
};

#endif // SERVERWIZARD_H
