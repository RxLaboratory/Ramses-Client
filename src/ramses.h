#ifndef RAMSES_H
#define RAMSES_H

#include "duqf-app/app-utils.h"
#include "dbinterface.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramstate.h"
#include "ramassetgroup.h"
#include "ramfiletype.h"
#include "ramapplication.h"
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
    // Tree base
    void setRamsesPath(const QString &ramsesPath);
    QString pathFromRamses(QString p) const;
    QString ramsesPath() const;
    QString usersPath() const;
    QString projectsPath() const;
    QString configPath() const;
    QString defaultUserPath(RamUser *u) const;
    QString defaultProjectPath(RamProject *p) const;
    // Tree Users
    QString path(RamUser *u) const;
    QDir dir(RamUser *u)const;
    QString currentUserPath() const;
    QDir currentUserDir() const;
    QString configPath(RamUser *u)const;
    QDir configDir(RamUser *u)const;
    QString currentUserConfigPath() const;
    QDir currentUserCondigDir() const;
    // Tree Projects
    QString path(RamProject *p) const;
    QDir dir(RamProject *p)const;
    QString configPath(RamProject *p) const;
    QDir configDir(RamProject *p) const;
    QString adminPath(RamProject *p) const;
    QDir adminDir(RamProject *p) const;
    QString preProdPath(RamProject *p) const;
    QDir preProdDir(RamProject *p) const;
    QString prodPath(RamProject *p) const;
    QDir prodDir(RamProject *p) const;
    QString postProdPath(RamProject *p) const;
    QDir postProdDir(RamProject *p) const;
    QString assetsPath(RamProject *p) const;
    QDir assetsDir(RamProject *p) const;
    QString shotsPath(RamProject *p) const;
    QDir shotsDir(RamProject *p) const;
    QString exportPath(RamProject *p) const;
    QDir exportDir(RamProject *p) const;
    // Tree steps
    QString path(RamStep *s) const;
    QDir dir(RamStep *s) const;
    // Tree assets
    QString path(RamAssetGroup *ag) const;
    QDir dir(RamAssetGroup *ag) const;
    QString path(RamAsset *a) const;
    QDir dir(RamAsset *a) const;
    // Users
    QList<RamUser *> users() const;
    RamUser *currentUser() const;
    RamUser *createUser();
    RamUser *user(QString uuid);
    void removeUser(QString uuid);
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();
    QString currentUserSettingsFile();
    // Projects
    QList<RamProject *> projects() const;
    RamProject *project(QString uuid) const;
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
    RamAssetGroup *assetGroup(QString uuid) const;
    // States
    QList<RamState *> states() const;
    RamState *createState();
    void removeState(RamObject *s);
    void removeState(QString uuid);
    // File Types
    QList<RamFileType*> fileTypes() const;
    RamFileType *fileType(const QString uuid) const;
    RamFileType *createFileType();
    void removeFileType(QString uuid); 
    // Applications
    QList<RamApplication *> applications() const;
    RamApplication *createApplication();
    void removeApplication(QString uuid);


public slots:
    void refresh();
    void removeApplication(RamObject *a);
    void removeFileType(RamObject *ft);

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
    void newFileType(RamFileType *);
    void newApplication(RamApplication *);

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
    //file types
    void gotFileTypes(QJsonArray fileTypes);
    //applications
    void gotApplications(QJsonArray applications);
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
    QSettings *_userSettings;

    DBInterface *_dbi;
    void login(QJsonObject user);
    /**
     * @brief True when loogged in, false otherwise.
     */
    bool _connected;

    // Tree
    QString _ramsesPath;
    QDir createPath(QString p) const;

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

    // File types
    QList<RamFileType*> _fileTypes;

    // Applications
    QList<RamApplication*> _applications;

};

#endif // RAMSES_H
