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

QStringList RamStatus::mainFiles() const
{
    return listFiles();
}

QString RamStatus::createFileFromTemplate(QString templateFileName) const
{

    QString filePath = m_step->templateFile( templateFileName );

    if (!QFileInfo::exists( filePath)) return "";

    // Generate destination name
    RamNameManager nm;
    nm.setFileName(filePath);
    nm.setProject( m_item->project()->shortName() );
    nm.setStep( m_step->shortName() );
    nm.setResource( nm.shortName() );
    if (m_item->objectType() == Asset) nm.setType("A");
    else if (m_item->objectType() == Shot) nm.setType("S");
    else nm.setType("G");
    nm.setShortName( m_item->shortName() );
    if (nm.resource() == "Template") nm.setResource("");

    QString destination = QDir(
                path(NoFolder, true)
                ).filePath( nm.fileName() );

     if (QFileInfo::exists( destination ))
     {
         nm.setResource( nm.resource() + "+new+");
         destination = QDir(
                     path(NoFolder, true)
                     ).filePath( nm.fileName() );
     }

     if (QFile::copy(filePath, destination)) return destination;
     return "";
}

int RamStatus::version() const
{
    return m_version;
}

QStringList RamStatus::versionFiles() const
{
    return listFiles(VersionsFolder);
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

QString RamStatus::restoreVersionFile(QString fileName) const
{
    QString restoredPath;
    RamNameManager nm;
    nm.setFileName( fileName );
    QString resource = nm.resource();
    QString v = QString::number( nm.version() );
    while(v.count() < 3)
        v = "0" + v;
    resource += "+restored-v" + v + "+";
    nm.setResource(resource);
    nm.setVersion(-1);

    QString versionPath = QDir( path(RamObject::VersionsFolder) ).filePath( fileName );
    restoredPath = QDir( path()).filePath( nm.fileName() );

    // Copy
    QFile::copy(versionPath, restoredPath);

    return restoredPath;

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
    QString difficulty = "medium";
    if (m_difficulty == VeryEasy) difficulty = "veryEasy";
    else if (m_difficulty == Easy) difficulty = "easy";
    else if (m_difficulty == Hard) difficulty = "hard";
    else if (m_difficulty == VeryHard) difficulty = "veryHard";
    m_dbi->updateStatus(
                m_uuid,
                m_state->uuid(),
                m_comment,
                m_version,
                m_completionRatio,
                m_published,
                assignedUser,
                timeSpent,
                m_date,
                difficulty,
                m_estimation);
}

void RamStatus::edit(bool show)
{
    if (!m_editReady)
    {
        ui_editWidget = new StatusEditWidget(this);
        setEditWidget(ui_editWidget);
        m_editReady = true;
        connect( ui_editWidget, SIGNAL(statusUpdated(RamState*,int,int,QString)),
                 this, SLOT(statusUpdated(RamState*,int,int,QString))
                 );
    }
    if (show) ui_editWidget->setStatus( this );
    showEdit(show);
}

void RamStatus::removeFromDB()
{
    m_dbi->removeStatus(m_uuid);
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
    this->assignUser(ui_editWidget->assignedUser());
    this->setPublished(ui_editWidget->isPublished());
    this->setDate(QDateTime::currentDateTime());
    this->setTimeSpent( ui_editWidget->timeSpent() );
    this->setEstimation( ui_editWidget->estimation() );
    this->setDifficulty( ui_editWidget->difficulty() );
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

float RamStatus::estimation() const
{
    return m_estimation;
}

float RamStatus::autoEstimation(int difficulty) const
{
    float estimation = 0.0;

    switch (difficulty)
    {
    case VeryEasy:
    {
        estimation = m_step->estimationVeryEasy();
        break;
    }
    case Easy:
    {
        estimation = m_step->estimationEasy();
        break;
    }
    case Medium:
    {
        estimation = m_step->estimationMedium();
        break;
    }
    case Hard:
    {
        estimation = m_step->estimationHard();
        break;
    }
    case VeryHard:
    {
        estimation = m_step->estimationVeryHard();
        break;
    }
    default:
    {
        estimation = m_step->estimationMedium();
    }
    }

    // Multiply by duration and num assets if shot
    if ( m_item->objectType() == RamObject::Shot)
    {
        RamShot *shot = qobject_cast<RamShot*>( m_item );
        if (m_step->estimationMethod() == RamStep::EstimatePerSecond)
            estimation *= shot->duration();
        RamAssetGroup *ag = m_step->estimationMultiplyGroup();
        if (ag)
        {
            // count assets
            int numAssets = 0;
            for (int i = 0; i < shot->assets()->count(); i++)
            {
                RamAsset *asset = qobject_cast<RamAsset*>( shot->assets()->at(i) );
                if (asset->assetGroup()->is(ag)) numAssets++;
            }
            if (numAssets > 0) estimation *= numAssets;
        }
    }
    return estimation;
}

float RamStatus::latenessRatio() const
{
    float completionRatio = m_completionRatio / 100.0;

    float estimation;
    if (m_estimation <= 0) estimation = autoEstimation();
    else estimation = m_estimation;

    float timeRatio = hoursToDays(m_timeSpent/3600) / estimation;

    return timeRatio / completionRatio;
}

float RamStatus::autoEstimation() const
{
    return autoEstimation(m_difficulty);
}

void RamStatus::setEstimation(float newEstimation)
{
    if (m_estimation == newEstimation) return;
    m_dirty = true;
    m_estimation = newEstimation;
    emit changed(this);
}

RamStatus::Difficulty RamStatus::difficulty() const
{
    return m_difficulty;
}

void RamStatus::setDifficulty(Difficulty newDifficulty)
{
    if (m_difficulty == newDifficulty) return;
    m_dirty = true;
    m_difficulty = newDifficulty;
    emit changed(this);
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
    return listFiles(PublishFolder);
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

QString RamStatus::previewImagePath() const
{
    QDir previewDir = path(RamObject::PreviewFolder);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg" << "*.gif";
    QStringList images = previewDir.entryList(filters, QDir::Files );

    if (images.count() == 0) return "";

    RamNameManager nm;

    RamProject *p = m_item->project();

    foreach(QString file, images)
    {
        if (nm.setFileName(file))
        {
            if (nm.project().toLower() != p->shortName().toLower()) continue;
            if (nm.step().toLower() != m_step->shortName().toLower()) continue;
            if (nm.shortName().toLower() != m_item->shortName().toLower()) continue;
            return previewDir.filePath( file );
        }
    }

    // Not found, return the first one
    return previewDir.filePath( images.at(0) );
}

QStringList RamStatus::previewFiles() const
{
    return listFiles(PreviewFolder);
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

RamStatus *RamStatus::copy(RamStatus *other, RamUser *user)
{
    RamStatus *status = new RamStatus(
                user,
                other->state(),
                other->step(),
                other->item());
    status->setCompletionRatio( other->completionRatio() );
    status->setVersion( other->version() );
    status->setTimeSpent( other->timeSpent() );
    status->assignUser( other->assignedUser() );
    status->setDifficulty( other->difficulty() );
    status->setEstimation( other->estimation() );
    status->item()->addStatus(status);
    return status;
}

float RamStatus::hoursToDays(int hours)
{
    // 1 day ( more or less )
    if (hours <= 0) return 0;
    if (hours == 1) return 0.15;
    if (hours == 2) return 0.25;
    if (hours == 3) return 0.4;
    if (hours == 4) return 0.5;
    if (hours == 5) return 0.75;
    if (hours <= 6) return 0.85;
    if (hours <= 9) return 1;

    // Approximately 2 days
    if (hours < 11) return 1.25;
    if (hours < 14) return 1.5;
    if (hours < 18) return 2;

    // More than that, 8h/day
    int intDays = hours / 8;
    return intDays;
}

int RamStatus::daysToHours(float days)
{
    // let's use an 8h-day
    return days*8;
}
