#include "ramses.h"

#include "datacrypto.h"
#include "ramuuid.h"

// STATIC //

Ramses *Ramses::_instance = nullptr;

Ramses *Ramses::instance()
{
    if(!_instance) _instance = new Ramses();
    return _instance;
}

// PUBLIC

RamUser *Ramses::login(QString username, QString password)
{
    // Hash password and login
    password = DataCrypto::instance()->generatePassHash(password);

    return loginHashed( username, password );
}

RamUser *Ramses::loginHashed(QString username, QString hashedPassword)
{
    QString uuid = m_dbi->login(username, hashedPassword);

    if (uuid == "")
    {
        logout("Invalid user name or password.");
        return nullptr;
    }

    refresh();

    // Set current user
    m_currentUser = RamUser::getObject(uuid, true);

    // Set current project
    if (m_currentUser)
    {
        QSettings *uSettings = m_currentUser->settings();
        QString projUuid = uSettings->value("ramses/currentProject", "").toString();
        if (projUuid != "") setCurrentProject( RamProject::getObject(projUuid) );
    }

    emit loggedIn(m_currentUser);

    return m_currentUser;
}

void Ramses::logout(QString reason)
{
    m_loggedin = false;

    setCurrentProject(nullptr);
    m_currentUser = nullptr;

    emit loggedOut(reason);
}

QString Ramses::pathFromRamses(QString p, bool create) const
{
    QString path;
    if (QFileInfo( p ).isRelative())
    {
        path = folderPath() + "/" + p;
    }
    else
    {
        path = p;
    }
    if (create)
        return createPath( p );
    else
        return p;
}

RamObjectList<RamUser *> *Ramses::users() const
{
    return m_users;
}

RamUser *Ramses::currentUser() const
{
    return m_currentUser;
}

RamUser *Ramses::ramsesUser()
{
    if (m_ramsesUser) return m_ramsesUser;

    m_ramsesUser = m_users->fromName("Ramses");

    if (!m_ramsesUser)
    {
        m_ramsesUser = new RamUser("Ramses", "Ramses Daemon");
        m_ramsesUser->updatePassword("", RamUuid::generateUuidString("Ramses"));
    }
    return m_ramsesUser;
}

RamUser *Ramses::removedUser()
{
    if (m_removedUser) return m_removedUser;

    m_removedUser = m_users->fromName("Removed");
    if (!m_removedUser)
    {
        m_removedUser = new RamUser("Removed", "Removed User");
        m_removedUser->updatePassword("", RamUuid::generateUuidString("Removed"));
    }
    return m_removedUser;
}

bool Ramses::isAdmin()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Admin;
}

bool Ramses::isProjectAdmin()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::ProjectAdmin;
}

bool Ramses::isLead()
{
    if (!m_currentUser) return false;
    return m_currentUser->role() >= RamUser::Lead;
}

RamStateList *Ramses::states() const
{
    return m_states;
}

RamState *Ramses::noState()
{
    if(m_noState) return m_noState;
    m_noState = m_states->fromName("NO");
    if (!m_noState)
    {
        m_noState = new RamState("NO", "Nothing to do");
        m_noState->setColor(QColor(36,36,36));
        m_states->append(m_noState);
    }
    return m_noState;
}

RamState *Ramses::todoState()
{
    if(m_todoState) return m_todoState;
    m_todoState =  m_states->fromName("TODO");
    if (!m_todoState)
    {
        m_todoState = new RamState("TODO", "To do");
        m_todoState->setColor(QColor(85,170,255));
        m_states->append(m_todoState);
    }
    return m_todoState;
}

RamState *Ramses::okState()
{
    if(m_okState) return m_okState;
    m_okState =  m_states->fromName("OK");
    if (!m_okState)
    {
        m_okState = new RamState("OK", "Finished");
        m_okState->setColor(QColor(0,170,0));
        m_states->append(m_okState);
    }
    return m_okState;
}

RamState *Ramses::stbState()
{
    if(m_stbState) return m_stbState;
    m_stbState = m_states->fromName("STB");
    if (!m_stbState)
    {
        m_stbState = new RamState("STB", "Stand by");
        m_stbState->setColor(QColor(168,168,168));
        m_states->append(m_stbState);
    }
    return m_stbState;
}

RamState *Ramses::wipState()
{
    if (m_wipState) return m_wipState;
    m_wipState = m_states->fromName("WIP");
    if (!m_wipState)
    {
        m_wipState = new RamState("WIP", "Work in progress");
        m_wipState->setColor(QColor(255,255,127));
        m_states->append(m_wipState);
    }
    return m_wipState;
}

RamObjectList<RamProject*> *Ramses::projects() const
{
    return m_projects;
}

RamProject *Ramses::currentProject() const
{
    return m_currentProject;
}

void Ramses::setCurrentProject(RamProject *project)
{
    if ( m_currentProject ) if ( m_currentProject->is(project) ) return;
    m_currentProject = project;

    if (m_currentUser)
    {
        QSettings *uSettings = m_currentUser->settings();
        uSettings->setValue("ramses/currentProject", m_currentProject->uuid() );
    }

    emit currentProjectChanged(m_currentProject);
}

void Ramses::setCurrentProject(QString shortName)
{
    setCurrentProject( m_projects->fromName(shortName) );
}

RamObjectList<RamTemplateStep *> *Ramses::templateSteps() const
{
    return m_templateSteps;
}

RamObjectList<RamTemplateAssetGroup *> *Ramses::templateAssetGroups() const
{
    return m_templateAssetGroups;
}

RamObjectList<RamFileType *> *Ramses::fileTypes() const
{
    return m_fileTypes;
}

RamObjectList<RamApplication *> *Ramses::applications() const
{
    return m_applications;
}

// PUBLIC SLOTS

void Ramses::refresh()
{
    if (!m_loggedin) return;

    // (Re)Load admin data

    m_users->reload( m_dbi->tableData("RamUser") );
    m_templateSteps->reload( m_dbi->tableData("RamTemplateStep") );
    m_templateAssetGroups->reload( m_dbi->tableData("RamTemplateAssetGroup") );
    m_states->reload( m_dbi->tableData("RamState") );
    m_fileTypes->reload( m_dbi->tableData("RamFileType") );
    m_applications->reload( m_dbi->tableData("RamApplication") );
    m_projects->reload( m_dbi->tableData("RamProject") );
}

// PROTECTED

QString Ramses::folderPath() const
{
    return m_settings.value("ramsesPath", QDir::homePath() + "/Ramses").toString();
}

// PRIVATE

Ramses::Ramses(QObject *parent):
    RamObject("RMSS", "Ramses", ObjectType::Ramses, parent, true)
{
    qDebug() << "Initialising Ramses";
    m_dbi = DBInterface::instance();

    m_users = new RamObjectList<RamUser*>("USRS", "Users", this, true);
    m_states = new RamStateList(this);
    m_projects = new RamObjectList<RamProject*>("PRJCTS", "Projects", this, true);
    m_templateSteps = new RamObjectList<RamTemplateStep*>("TPLTSTPS", "Template steps", this, true);
    m_templateAssetGroups = new RamObjectList<RamTemplateAssetGroup*>("TPLTAGS", "Template asset groups", this, true);
    m_fileTypes = new RamObjectList<RamFileType*>("FLTPS", "File types", this, true);
    m_applications = new RamObjectList<RamApplication*>("APPS", "Applications", this, true);

    this->setObjectName( "Ramses Class" );

    qDebug() << "Ramses Ready!";
}

QDir Ramses::createDir(QString p) const
{
    QDir d(p);
    if (p != "") d.mkpath(".");
    return d;
}

QString Ramses::createPath(QString p) const
{
    QDir d = createDir(p);
    return d.absolutePath();
}



































