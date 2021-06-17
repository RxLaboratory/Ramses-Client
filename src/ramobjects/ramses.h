#ifndef RAMSES_H
#define RAMSES_H

#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramstate.h"
#include "ramassetgroup.h"
#include "ramfiletype.h"
#include "ramapplication.h"
#include "ramstatelist.h"
#include "dbisuspender.h"
#include "duqf-app/app-utils.h"

#include <QObject>
#include <QtDebug>

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
    void setCurrentUser(RamUser *u);
    RamObjectList *users() const;
    RamUser *currentUser() const;
    RamUser *ramUser();
    void setRamUser(RamUser *user);
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();
    QString currentUserSettingsFile();
    // States
    RamState *noState();
    RamState *todoState();
    RamState *okState();
    // Projects
    RamObjectList *projects() const;
    RamProject *createProject();
    RamProject *currentProject() const;
    void setCurrentProject(RamProject *currentProject);
    void setCurrentProject(QString uuidOrShortName);
    // Template Steps
    RamObjectList *templateSteps() const;
    RamStep *createTemplateStep();
    // Template Asset Groups
    RamObjectList *templateAssetGroups() const;
    RamAssetGroup *createTemplateAssetGroup();
    // States
    RamStateList *states() const;
    // File Types
    RamObjectList *fileTypes() const;
    RamFileType *createFileType();
    // Applications
    RamObjectList *applications() const;
    RamApplication *createApplication();

public slots:
    void init();
    void refresh();
    void projectReady(QString uuid);
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

    /**
     * @brief True when loogged in, false otherwise.
     */
    bool _connected;

    // Tree
    QString _ramsesPath;
    QDir createDir(QString p) const;
    QString createPath(QString p) const;

    // Users
    RamObjectList *_users;
    RamUser *_currentUser;
    QString _currentUserShortName;
    RamUser *_ramUser;

    // Projects
    RamObjectList *_projects;
    RamProject *_currentProject;

    // Template steps
    RamObjectList *_templateSteps;

    // Template asset groups
    RamObjectList *_templateAssetGroups;

    // States
    RamStateList *_states;
    RamState *_noState;
    RamState *_okState;
    RamState *_todoState;

    // File types
    RamObjectList *_fileTypes;

    // Applications
    RamObjectList *_applications;
};

#endif // RAMSES_H
