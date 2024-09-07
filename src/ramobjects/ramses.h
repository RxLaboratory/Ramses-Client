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

    static Ramses *i();

    // OTHER METHODS //

    /**
     * @brief loadDatabase Tells Ramses that the database is ready
     */
    void loadDatabase();

    // Tree base
    QString pathFromRamses(QString p = "", bool create = false) const;

    // Applications
    DBTableModel *applications() const;
    // File Types
    DBTableModel *fileTypes() const;
    // Projects
    DBTableModel *projects() const;
    RamProject *project() const;
    // States
    DBTableModel *states() const;
    RamState *noState();
    RamState *todoState();
    RamState *okState();
    RamState *stbState();
    RamState *wipState();
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
    void ready();
    void roleChanged(RamUser::UserRole);

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
    DBTableModel *m_fileTypes = nullptr;
    DBTableModel *m_projects = nullptr;
    DBTableModel *m_states = nullptr;
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
