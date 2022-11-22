#ifndef RAMSES_H
#define RAMSES_H

#include "dbtablemodel.h"
#include "dbinterface.h"
#include "ramuser.h"
#include "ramproject.h"
#include "ramstate.h"

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
    void setUserUuid(QString uuid);
    void setUser(RamUser *u);

    // Tree base
    void setRamsesPath(QString p);
    QString pathFromRamses(QString p = "", bool create = false) const;

    // Applications
    DBTableModel *applications() const;
    // Assets
    DBTableModel *assets() const;
    // Asset Groups
    DBTableModel *assetGroups() const;
    // File Types
    DBTableModel *fileTypes() const;
    // Pipes
    DBTableModel *pipes() const;
    // Pipe Files
    DBTableModel *pipeFiles() const;
    // Projects
    DBTableModel *projects() const;
    RamProject *currentProject() const;
    void setCurrentProject(RamProject *project);
    void setCurrentProject(QString shortName);
    void setCurrentProjectUuid(QString uuid);
    // Schedule Comments
    DBTableModel *scheduleComments() const;
    // Schedule
    DBTableModel *schedule() const;
    // Sequences
    DBTableModel *sequences() const;
    // Shots
    DBTableModel *shots() const;
    // States
    DBTableModel *states() const;
    RamState *noState();
    RamState *todoState();
    RamState *okState();
    RamState *stbState();
    RamState *wipState();
    // Steps
    DBTableModel *steps() const;
    // Template Asset Groups
    DBTableModel *templateAssetGroups() const;
    // Template Steps
    DBTableModel *templateSteps() const;
    // Users
    DBTableModel *users() const;
    RamUser *currentUser() const;
    RamUser *ramsesUser();
    RamUser *removedUser();
    bool isAdmin();
    bool isProjectAdmin();
    bool isLead();

signals:
    void userChanged(RamUser*);
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
    DBTableModel *m_applications = nullptr;
    DBTableModel *m_assets = nullptr;
    DBTableModel *m_assetGroups = nullptr;
    DBTableModel *m_fileTypes = nullptr;
    DBTableModel *m_pipes = nullptr;
    DBTableModel *m_pipeFiles = nullptr;
    DBTableModel *m_projects = nullptr;
    DBTableModel *m_scheduleComments = nullptr;
    DBTableModel *m_schedule = nullptr;
    DBTableModel *m_sequences = nullptr;
    DBTableModel *m_shots = nullptr;
    DBTableModel *m_states = nullptr;
    DBTableModel *m_steps = nullptr;
    DBTableModel *m_templateAssetGroups = nullptr;
    DBTableModel *m_templateSteps = nullptr;
    DBTableModel *m_users = nullptr;;

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
