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
#include "ramstatelist.h"
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
    // Tree shots
    QString path(RamShot *s) const;
    QDir dir(RamShot *s) const;
    // Users
    RamObjectList *users() const;
    RamUser *currentUser() const;
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();
    QString currentUserSettingsFile();
    // Projects
    RamObjectList *projects() const;
    RamProject *project(QString uuid) const;
    RamProject *createProject();
    RamProject *currentProject() const;
    void setCurrentProject(RamProject *currentProject, bool updateData=true);
    void setCurrentProject(QString uuidOrShortName, bool updateData=true);
    // Template Steps
    RamObjectList *templateSteps() const;
    RamStep *createTemplateStep();
    RamStep *templateStep(QString uuid);
    // Template Asset Groups
    RamObjectList *templateAssetGroups() const;
    RamAssetGroup *createTemplateAssetGroup();
    RamAssetGroup *templateAssetGroup(QString uuid);
    // Asset groups
    RamAssetGroup *assetGroup(QString uuid) const;
    // Sequences
    RamSequence *sequence(QString uuid) const;
    // States
    RamStateList *states() const;
    // File Types
    RamObjectList *fileTypes() const;
    RamFileType *fileType(const QString uuid) const;
    RamFileType *createFileType();
    // Applications
    RamObjectList *applications() const;
    RamApplication *application(QString uuid);
    RamApplication *createApplication();

public slots:
    void refresh();
    // Users
    RamUser *createUser();
    // States
    RamState *createState();

signals:
    void loggedIn(RamUser*);
    void loggedOut();
    void currentProjectChanged(RamProject*);
    void newStep(RamStep *);
    void newAssetGroup(RamAssetGroup *);

protected:
    static Ramses *_instance;

private slots:
    void newData(QJsonObject data);
    //users
    void gotUsers(QJsonArray users);
    //projects
    void gotProjects(QJsonArray projects);
    QString gotProject(QJsonObject newP);
    //template steps
    void gotTemplateSteps(QJsonArray steps);
    //template asset groups
    void gotTemplateAssetGroups(QJsonArray assetGroups);
    //states
    void gotStates(QJsonArray states);
    //file types
    void gotFileTypes(QJsonArray fileTypes);
    //applications
    void gotApplications(QJsonArray applications);
    // steps
    void gotSteps(QJsonArray steps, RamProject *project);
    // asset groups
    void gotAssetGroups(QJsonArray assetGroups, RamProject *project);
    // assets
    void gotAssets(QJsonArray assets, RamAssetGroup *assetGroup, RamProject *project);
    // sequences
    void gotSequences(QJsonArray sequences, RamProject *project);
    QString gotSequence(QJsonObject newS, RamProject *project);
    // shots
    void gotShots(QJsonArray shots, RamSequence *sequence, RamProject *project);
    QString gotShot(QJsonObject newS, RamSequence *sequence, RamProject *project);
    // status
    void gotStatusHistory(QJsonArray statusHistory, RamItem *item, RamProject *project);
    QString gotStatus(QJsonObject newS, RamItem *item, RamProject *project);
    // pipes
    void gotPipes(QJsonArray pipes, RamProject *project);
    QString gotPipe(QJsonObject newP, RamProject *project);
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
    RamObjectList *_users;
    RamUser *_currentUser;
    QString _currentUserShortName;

    // Projects
    RamObjectList *_projects;
    RamProject *_currentProject;

    // Template steps
    RamObjectList *_templateSteps;

    // Template asset groups
    RamObjectList *_templateAssetGroups;

    // States
    RamStateList *_states;

    // File types
    RamObjectList *_fileTypes;

    // Applications
    RamObjectList *_applications;

};

#endif // RAMSES_H
