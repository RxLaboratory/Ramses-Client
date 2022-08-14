#ifndef RAMSES_H
#define RAMSES_H

#include "dbinterface.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstep.h"
#include "ramstate.h"
#include "ramtemplateassetgroup.h"
#include "data-models/ramstatelist.h"

#include <QObject>
#include <QtDebug>

class Ramses : public RamObject
{
    Q_OBJECT
public:

    // STATIC METHODS //

    static Ramses *instance();

    // OTHER METHODS //

    // User
    RamUser *login(QString username, QString password);
    RamUser *loginHashed(QString username, QString hashedPassword);
    void logout(QString reason="");

    // Tree base
    void setRamsesPath(QString p);
    QString pathFromRamses(QString p = "", bool create = false) const;

    // Users
    RamObjectList *users() const;
    RamUser *currentUser() const;
    RamUser *ramsesUser();
    RamUser *removedUser();
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();

    // States
    RamStateList *states() const;
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

    // Template Steps
    RamObjectList *templateSteps() const;

    // Template Asset Groups
    RamObjectList *templateAssetGroups() const;

    // File Types
    RamObjectList *fileTypes() const;

    // Applications
    RamObjectList *applications() const;

public slots:
    void refresh();

signals:
    void loggedIn(RamUser*);
    void loggedOut(QString);
    void currentProjectChanged(RamProject*);

protected:
    static Ramses *_instance;
    virtual QString folderPath() const override;

private:

    // METHODS //

    /**
     * @brief Ramses is the class managing all data. It is a singleton
     * @param parent
     */
    explicit Ramses(QObject *parent = nullptr);

    QDir createDir(QString p) const;
    QString createPath(QString p) const;

    // ATTRIBUTES //

    // DATA (lists)
    RamObjectList *m_users;
    RamObjectList *m_templateSteps;
    RamObjectList *m_templateAssetGroups;
    RamStateList *m_states;
    RamObjectList *m_fileTypes;
    RamObjectList *m_applications;
    RamObjectList *m_projects;

    // Users
    RamUser *m_currentUser = nullptr;
    RamUser *m_ramsesUser = nullptr;
    RamUser *m_removedUser = nullptr;
    bool m_loggedin = false;

    // States
    RamState *m_noState = nullptr;
    RamState *m_okState = nullptr;
    RamState *m_todoState = nullptr;
    RamState *m_stbState = nullptr;
    RamState *m_wipState = nullptr;

    // Projects
    RamProject *m_currentProject = nullptr;

    // LOW LEVEL
    DBInterface *m_dbi;
    QSettings m_settings;
};

#endif // RAMSES_H
