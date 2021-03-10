#ifndef RAMSES_H
#define RAMSES_H

#include "duqf-app/app-utils.h"
#include "dbinterface.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramstate.h"
#include "ramassetgroup.h"
#include "dbisuspender.h"

#include <QObject>

class Ramses : public QObject
{
    Q_OBJECT
public:
    static Ramses *instance();
    ~Ramses();
    void login(QString username, QString password);
    void logout();
    bool isConnected() const;
    QString mainPath() const;
    // Tree
    void setMainPath(const QString &mainPath);
    QString pathFromMain(QString p) const;
    QString usersPath() const;
    QString userPath(RamUser *u) const;
    QString defaultUserPath(RamUser *u) const;
    QString projectsPath() const;
    QString projectPath(RamProject *p) const;
    QString defaultProjectPath(RamProject *p);
    // Users
    QList<RamUser *> users() const;
    RamUser *currentUser() const;
    RamUser *createUser();
    RamUser *user(QString uuid);
    void removeUser(QString uuid);
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();
    // Projects
    QList<RamProject *> projects() const;
    RamProject *project(QString uuid);
    RamProject *createProject();
    void removeProject(QString uuid);
    RamProject *currentProject() const;
    void setCurrentProject(RamProject *currentProject);
    void setCurrentProject(QString uuid);
    // Template Steps
    QList<RamStep *> templateSteps() const;
    RamStep *createTemplateStep();
    void removeTemplateStep(QString uuid);
    RamStep *templateStep(QString uuid);
    // Template Asset Groups
    QList<RamAssetGroup *> templateAssetGroups() const;
    RamAssetGroup *createTemplateAssetGroup();
    void removeTemplateAssetGroup(QString uuid);
    RamAssetGroup *templateAssetGroup(QString uuid);
    // Asset groups
    RamAssetGroup *assetGroup(QString uuid);
    // States
    QList<RamState *> states() const;
    RamState *createState();
    void removeState(QString uuid);

public slots:
    void refresh();

signals:
    void loggedIn(RamUser*);
    void loggedOut();
    void projectChanged(RamProject*);
    void newUser(RamUser *user);
    void newProject(RamProject *project);
    void newTemplateStep(RamStep *step);
    void newTemplateAssetGroup(RamAssetGroup *assetGroup);
    void newState(RamState *state);
    void newStep(RamStep *);
    void newAssetGroup(RamAssetGroup *);

protected:
    static Ramses *_instance;

private slots:
    void newData(QJsonObject data);
    //users
    void gotUsers(QJsonArray users);
    void userDestroyed(QObject *o);
    //projects
    void gotProjects(QJsonArray projects);
    void projectDestroyed(QObject *o);
    //template steps
    void gotTemplateSteps(QJsonArray steps);
    void templateStepDestroyed(QObject *o);
    //template asset groups
    void gotTemplateAssetGroups(QJsonArray assetGroups);
    void templateAssetGroupDestroyed(QObject *o);
    //states
    void gotStates(QJsonArray states);
    void stateDestroyed(QObject *o);
    // steps
    void gotSteps(QJsonArray steps, RamProject *project);
    // asset groups
    void gotAssetGroups(QJsonArray assetGroups, RamProject *project);
    // assets
    void gotAssets(QJsonArray assets, RamAssetGroup *assetGroup);
    // sequences
    void gotSequences(QJsonArray sequences, RamProject *project);
    //TODO This should be modified when implementing offline version
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);
private:
    /**
     * @brief Ramses is the class managing all data. It is a singleton
     * @param parent
     */
    explicit Ramses(QObject *parent = nullptr);

    QSettings _settings;

    DBInterface *_dbi;
    void login(QJsonObject user);
    /**
     * @brief True when loogged in, false otherwise.
     */
    bool _connected;

    // Tree
    QString _mainPath;

    // Users
    QList<RamUser *> _users;
    RamUser *_currentUser;
    QString _currentUserShortName;

    // Projects
    QList<RamProject *> _projects;
    RamProject *_currentProject;

    // Template steps
    QList<RamStep *> _templateSteps;

    // Template asset groups
    QList<RamAssetGroup *> _templateAssetGroups;

    // States
    QList<RamState *> _states;

};

#endif // RAMSES_H
