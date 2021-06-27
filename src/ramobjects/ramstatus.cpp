#include "ramstatus.h"

#include "ramitem.h"
#include "statuseditwidget.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, QString uuid):
    RamObject("", "", uuid, item)
{
    m_user = user;
    m_state = state;
    m_step = step;
    m_item = item;
    m_completionRatio = m_state->completionRatio();
    m_date = QDateTime::currentDateTimeUtc();

    setObjectType(Status);

    connect(user, SIGNAL( removed(RamObject*)), this, SLOT(userRemoved()));
    connect(item, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
    connect(step, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
    m_stateConnection = connect(state, SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));

    this->setObjectName( "RamStatus" );
}

RamStatus::~RamStatus()
{
    m_dbi->removeStatus(m_uuid);
}

int RamStatus::completionRatio() const
{
    return m_completionRatio;
}

void RamStatus::setCompletionRatio(int completionRatio)
{
    if (completionRatio == m_completionRatio) return;
    m_dirty = true;
    m_completionRatio = completionRatio;
    emit changed(this);
}

RamUser *RamStatus::user() const
{
    return m_user;
}

RamState *RamStatus::state() const
{
    return m_state;
}

void RamStatus::setState(RamState *state)
{
    disconnect(m_stateConnection);

    if (!state && !m_state) return;
    if (state && state->is(m_state)) return;
    m_dirty = true;
    m_state = state;

    m_stateConnection = connect(state, SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));

    emit changed(this);
}

int RamStatus::version() const
{
    return m_version;
}

QStringList RamStatus::versionFiles() const
{
    QDir versionDir( path(VersionsFolder) );
    QStringList files = versionDir.entryList(QDir::Files);
    files.removeAll( RamFileMetaDataManager::metaDataFileName() );
    return files;
}

QStringList RamStatus::versionFiles(QString resource) const
{
    QStringList files;

    RamNameManager nm;

    RamProject *p = m_item->project();

    // look for files with the same resource
    foreach(QString file, versionFiles())
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            if (nm.resource() == resource) files << file;
        }
    }
    return files;
}

int RamStatus::latestVersion() const
{
    QStringList files;

    RamNameManager nm;

    RamProject *p = m_item->project();

    int v = 0;

    // look for files with the same resource
    foreach(QString file, versionFiles())
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            if (nm.version() > v) v = nm.version();
        }
    }
    return v;
}

int RamStatus::latestVersion(QString resource) const
{
    QStringList files;

    RamNameManager nm;

    int v = 0;

    RamProject *p = m_item->project();

    // look for files with the same resource
    foreach(QString file, versionFiles())
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            if (nm.resource() == resource && nm.version() > v) v = nm.version();
        }
    }
    return v;
}

void RamStatus::setVersion(int version)
{
    if (m_version == version) return;
    m_dirty = true;
    m_version = version;
    emit changed(this);
}

RamStep *RamStatus::step() const
{
    return m_step;
}

RamItem *RamStatus::item() const
{
    return m_item;
}

void RamStatus::update()
{
    if(!m_dirty) return;
    RamObject::update();
    if (!m_state) return;
    QString assignedUser = "NULL";
    if (m_assignedUser) assignedUser = m_assignedUser->uuid();
    qint64 timeSpent = -1;
    if (m_timeSpent > 0) timeSpent = m_timeSpent;
    m_dbi->updateStatus(
                m_uuid,
                m_state->uuid(),
                m_comment,
                m_version,
                m_completionRatio,
                m_published,
                assignedUser,
                timeSpent,
                m_date);
}

void RamStatus::edit(bool show)
{
    if (!m_editReady)
    {
        m_editWidget = new StatusEditWidget(this);
        setEditWidget(m_editWidget);
        m_editReady = true;
        connect( m_editWidget, SIGNAL(statusUpdated(RamState*,int,int,QString)),
                 this, SLOT(statusUpdated(RamState*,int,int,QString))
                 );
    }
    if (show) m_editWidget->setStatus( this );
    showEdit(show);
}

QString RamStatus::folderPath() const
{
    RamProject *project = m_item->project();
    QString type = "_G_";
    if (m_item->objectType() == RamObject::Shot) type = "_S_";
    else if (m_item->objectType() == RamObject::Asset) type = "_A_";
    return m_item->path() + "/" + project->shortName() + type + m_item->shortName() + "_" + m_step->shortName();
}

void RamStatus::statusUpdated(RamState *state, int completion, int version, QString comment)
{
    this->setState(state);
    this->setCompletionRatio(completion);
    this->setVersion(version);
    this->setComment(comment);
    this->assignUser(m_editWidget->assignedUser());
    this->setPublished(m_editWidget->isPublished());
    this->setDate(QDateTime::currentDateTime());
    this->setTimeSpent( m_editWidget->timeSpent() );
    update();
    showEdit(false);
}

void RamStatus::stateRemoved()
{
    this->setState( Ramses::instance()->wipState() );
}

void RamStatus::userRemoved()
{
    m_user = Ramses::instance()->removedUser();
    m_dbi->setStatusUser( m_uuid, m_user->uuid() );
}

void RamStatus::assignedUserRemoved()
{
    assignUser(nullptr);
}

RamUser *RamStatus::assignedUser() const
{
    return m_assignedUser;
}

void RamStatus::assignUser(RamUser *assignedUser)
{
    disconnect(m_assignedUserConnection);

    if (!assignedUser && !m_assignedUser) return;
    if (assignedUser)
        if (assignedUser->is(m_assignedUser)) return;
    m_dirty = true;
    m_assignedUser = assignedUser;

    if (m_assignedUser) m_assignedUserConnection = connect(assignedUser, SIGNAL(removed(RamObject*)), this, SLOT(assignedUserRemoved()));

    emit changed(this);
}

qint64 RamStatus::timeSpent()
{
    if (m_timeSpent == 0)
    {
        // Let's detect!
        RamFileMetaDataManager mdm( this->path(RamObject::VersionsFolder) );
        if (!mdm.isValid()) return m_timeSpent;
        m_timeSpent = mdm.getTimeRange();
    }
    return m_timeSpent;
}

void RamStatus::setTimeSpent(const float &ts)
{
    if (ts == m_timeSpent) return;
    m_dirty = true;
    m_manualTimeSpent = true;
    m_timeSpent = ts;
    emit changed(this);
}

bool RamStatus::isTimeSpentManual() const
{
    return m_manualTimeSpent;
}

bool RamStatus::isPublished() const
{
    return m_published;
}

bool RamStatus::checkPublished( int version ) const
{
    // Check if there's a published file corresponding to the version
    QStringList files = publishedFiles("");
    if (version == -1 ) return files.count() > 0;

    RamFileMetaDataManager mdm( path( RamObject::PublishFolder ));

    foreach(QString file, files)
    {
        if ( mdm.getVersion(file) == version) return true;
    }
    return false;
}

QStringList RamStatus::publishedFiles() const
{
    QDir versionDir( path(PublishFolder) );
    QStringList files = versionDir.entryList(QDir::Files);
    files.removeAll( RamFileMetaDataManager::metaDataFileName() );
    return files;
}

QStringList RamStatus::publishedFiles(QString resource) const
{
    QStringList files;

    RamNameManager nm;

    RamProject *p = m_item->project();

    // look for files with the same resource
    foreach(QString file, publishedFiles())
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            if (nm.resource() == resource) files << file;
        }
    }
    return files;
}

void RamStatus::setPublished(bool published)
{
    if (published == m_published) return;
    m_dirty = true;
    m_published = published;
    emit changed(this);
}

QDateTime RamStatus::date() const
{
    return m_date;
}

void RamStatus::setDate(const QDateTime &date)
{
    if (m_date == date) return;
    m_dirty = true;
    m_date = date;
    emit changed(this);
}

RamStatus *RamStatus::status(QString uuid)
{
    return qobject_cast<RamStatus*>( RamObject::obj(uuid) );
}
