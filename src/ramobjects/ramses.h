#ifndef RAMSES_H
#define RAMSES_H

#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramstate.h"
#include "ramassetgroup.h"
#include "ramfiletype.h"
#include "ramapplication.h"
#include "ramshot.h"
#include "data-models/ramstatelist.h"
#include "dbisuspender.h"
#include "duqf-app/app-utils.h"

#include <QObject>
#include <QtDebug>

class Ramses : public RamObject
{
    Q_OBJECT
public:
    static Ramses *instance();
    ~Ramses();
    // User
    void login(QString username, QString password);
    void logout();
    // Server connection
    bool isOnline() const;
    // Tree base
    void setRamsesPath(const QString &ramsesPath);
    QString pathFromRamses(QString p, bool create = false) const;
    // Users
    void setCurrentUser(RamUser *u);
    RamObjectList *users() const;
    RamUser *currentUser() const;
    RamUser *ramUser();
    RamUser *removedUser();
    void setRamUser(RamUser *user);
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();
    // States
    RamState *noState();
    RamState *todoState();
    RamState *okState();
    RamState *stbState();
    RamState *wipState();
    // Projects
    RamObjectList *projects() const;
    RamProject *currentProject() const;
    void setCurrentProject(RamProject *project);
    void setCurrentProject(QString shortName);
    void setCurrentProjectUuid(QString uuid);
    // Template Steps
    RamObjectList *templateSteps() const;
    // Template Asset Groups
    RamObjectList *templateAssetGroups() const;
    // States
    RamStateList *states() const;
    // File Types
    RamObjectList *fileTypes() const;
    // Applications
    RamObjectList *applications() const;

public slots:
    void init();
    void refresh();
    void projectReady(QString uuid);
    void setOnline();
    void setOffline();

signals:
    void loggedIn(RamUser*);
    void loggedOut();
    void currentProjectChanged(RamProject*);
    void newStep(RamStep *);
    void newAssetGroup(RamAssetGroup *);
    void busy(bool);

protected:
    static Ramses *_instance;
    virtual QString folderPath() const override;

private slots:
    //TODO This should be modified when implementing offline version
    void dbiConnectionStatusChanged(NetworkUtils::NetworkStatus s);

private:
    /**
     * @brief Ramses is the class managing all data. It is a singleton
     * @param parent
     */
    explicit Ramses(QObject *parent = nullptr);

    QSettings m_settings;

    DBInterface *m_dbi;

    /**
     * @brief True when loogged in, false otherwise.
     */
    bool m_connected;

    // Tree
    QString m_ramsesPath;
    QDir createDir(QString p) const;
    QString createPath(QString p) const;

    // Users
    RamObjectList *m_users;
    RamUser *m_currentUser;
    RamUser *m_ramUser;
    RamUser *m_removedUser;

    // Projects
    RamObjectList *m_projects;
    RamProject *m_currentProject;

    // Template steps
    RamObjectList *m_templateSteps;

    // Template asset groups
    RamObjectList *m_templateAssetGroups;

    // States
    RamStateList *m_states;
    RamState *m_noState = nullptr;
    RamState *m_okState = nullptr;
    RamState *m_todoState = nullptr;
    RamState *m_stbState = nullptr;
    RamState *m_wipState = nullptr;

    // File types
    RamObjectList *m_fileTypes;
    // Applications
    RamObjectList *m_applications;
};

#endif // RAMSES_H
