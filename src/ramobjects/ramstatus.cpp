#include "ramstatus.h"

#include "ramitem.h"
#include "statuseditwidget.h"
#include "ramses.h"

RamStatus::RamStatus(RamUser *user, RamState *state, RamStep *step, RamItem *item, bool computeEstimation, QString uuid):
    RamObject("", "", uuid, item)
{
    m_icon = ":/icons/status";
    m_editRole = Standard;

    m_user = user;
    m_state = state;
    m_step = step;
    m_item = item;
    m_completionRatio = m_state->completionRatio();
    m_date = QDateTime::currentDateTimeUtc();

    if (m_state->is( Ramses::instance()->noState() )) computeEstimation = false;
    if (computeEstimation) m_step->computeEstimation();

    setObjectType(Status);

    connect(user, SIGNAL( removed(RamObject*)), this, SLOT(userRemoved()));
    connect(item, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
    connect(step, SIGNAL( removed(RamObject*)), this, SLOT(remove()));
    m_stateConnection = connect(state, SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));

    this->setObjectName( "RamStatus" );
}

QString RamStatus::shortName() const
{
    QString n = "Status";
    if (m_item) n = m_item->shortName();
    if (m_step) n = n + " | " + m_step->shortName();
    return n;
}

QString RamStatus::name() const
{
    QString n = "Status";
    if (m_item) n = m_item->name();
    if (m_step) n = n + " | " + m_step->name();
    return n;
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
    m_step->computeEstimation();
    emit dataChanged(this);
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
    if (!state) return;

    m_completionRatio = state->completionRatio();

    m_step->computeEstimation();

    m_stateConnection = connect(state, SIGNAL(removed(RamObject*)), this, SLOT(stateRemoved()));

    emit dataChanged(this);
}

RamWorkingFolder RamStatus::workingFolder() const
{
    return RamWorkingFolder(this->path());
}

QString RamStatus::createFileFromTemplate(QString filePath) const
{
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

QString RamStatus::createFileFromResource(QString filePath) const
{
    // Get infos
    RamNameManager nm;
    nm.setFileName(filePath);

    RamWorkingFolder wf( QFileInfo(filePath).path() );

    // Get the latest publish if any
    QStringList templateFiles = wf.publishedFiles(nm.resource());
    foreach (QString tFile, templateFiles)
    {
        // Check the file extension, which should be the same
        if ( QFileInfo(tFile).suffix() == nm.extension()) return createFileFromTemplate(tFile);
    }
    // Try with the first published file
    if (templateFiles.count() > 0) return createFileFromTemplate(templateFiles.at(0));

    // Use the given file path
    return createFileFromTemplate( filePath );
}

int RamStatus::version() const
{
    return m_version;
}

void RamStatus::setVersion(int version)
{
    if (m_version == version) return;
    m_dirty = true;
    m_version = version;
    emit dataChanged(this);
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
    float estim = -1;
    if (!m_useAutoEstimation) estim = m_goal;
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
                estim);
}

void RamStatus::edit(bool show)
{
    if (!m_editReady)
    {
        ui_editWidget = new StatusEditWidget(this);
        setEditWidget(ui_editWidget);
        m_editReady = true;
    }
    if (show)
    {
        ui_editWidget->setObject( this );
        showEdit();
    }
}

void RamStatus::removeFromDB()
{
    m_dbi->removeStatus(m_uuid);
}

void RamStatus::remove(bool updateDB)
{
    RamObject::remove(updateDB);
    if ( !m_state->is( Ramses::instance()->noState() ) ) m_step->computeEstimation();
}

QString RamStatus::folderPath() const
{
    RamProject *project = m_item->project();
    QString type = "_G_";
    if (m_item->objectType() == RamObject::Shot) type = "_S_";
    else if (m_item->objectType() == RamObject::Asset) type = "_A_";
    return m_item->path() + "/" + project->shortName() + type + m_item->shortName() + "_" + m_step->shortName();
}

void RamStatus::stateRemoved()
{
    this->setState( Ramses::instance()->wipState() );
}

void RamStatus::userRemoved()
{
    m_user = Ramses::instance()->removedUser();
    if (m_user) m_dbi->setStatusUser( m_uuid, m_user->uuid() );
}

void RamStatus::assignedUserRemoved()
{
    assignUser(nullptr);
}

bool RamStatus::useAutoEstimation() const
{
    if (m_state->shortName() == "NO") return true;
    return m_useAutoEstimation;
}

void RamStatus::setUseAutoEstimation(bool newAutoEstimation)
{
    if (m_state->shortName() == "NO") return;
    m_dirty = true;
    m_useAutoEstimation = newAutoEstimation;
    if (!m_useAutoEstimation && m_goal <= 0) m_goal = estimation();
    m_step->computeEstimation();
    emit dataChanged(this);
}

float RamStatus::goal() const
{
    // If state is none, 0!
    RamState *noState = Ramses::instance()->noState();
    if (noState->is(m_state)) return 0.0;

    // can't be < 0
    if (m_goal < 0) return 0.0;

    return m_goal;
}

void RamStatus::setGoal(float newGoal)
{
    if (m_goal == newGoal) return;
    m_dirty = true;
    m_goal = newGoal;
    m_useAutoEstimation = m_goal < 0;
    m_step->computeEstimation();
    emit dataChanged(this);
}

float RamStatus::estimation() const
{
    return estimation(m_difficulty);
}

float RamStatus::estimation(int difficulty) const
{
    float est = 0.0;

    // If state is none, 0!
    RamState *noState = Ramses::instance()->noState();
    if (noState->is(m_state)) return 0.0;

    switch (difficulty)
    {
    case VeryEasy:
    {
        est = m_step->estimationVeryEasy();
        break;
    }
    case Easy:
    {
        est = m_step->estimationEasy();
        break;
    }
    case Medium:
    {
        est = m_step->estimationMedium();
        break;
    }
    case Hard:
    {
        est = m_step->estimationHard();
        break;
    }
    case VeryHard:
    {
        est = m_step->estimationVeryHard();
        break;
    }
    default:
    {
        est = m_step->estimationMedium();
    }
    }

    // Multiply by duration and num assets if shot
    if ( m_item->objectType() == RamObject::Shot)
    {
        RamShot *shot = qobject_cast<RamShot*>( m_item );
        if (m_step->estimationMethod() == RamStep::EstimatePerSecond)
            est *= shot->duration();
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
            if (numAssets > 0) est *= numAssets;
        }
    }
    return est;
}

float RamStatus::latenessRatio() const
{
    float completionRatio = m_completionRatio / 100.0;

    float est;
    if (useAutoEstimation()) est = estimation();
    else est = m_goal;

    if (est <= 0) return 1;
    if (completionRatio <= 0) return 1;

    float timeRatio = hoursToDays(m_timeSpent/3600) / est;

    return timeRatio / completionRatio;
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
    emit dataChanged(this);
}

RamUser *RamStatus::assignedUser() const
{
    return m_assignedUser;
}

void RamStatus::assignUser(RamUser *assignedUser)
{
    if (!assignedUser && !m_assignedUser) return;
    if (assignedUser)
        if (assignedUser->is(m_assignedUser)) return;

    disconnect(m_assignedUserConnection);

    if (this->item()->shortName() == "sq01sh001")
    {
        qDebug() << assignedUser;
        if (assignedUser) qDebug() << assignedUser->name();
    }

    m_dirty = true;
    m_assignedUser = assignedUser;

    if (m_assignedUser) m_assignedUserConnection = connect(assignedUser, SIGNAL(removed(RamObject*)), this, SLOT(assignedUserRemoved()));

    emit dataChanged(this);
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
    m_step->computeEstimation();
    emit dataChanged(this);
}

bool RamStatus::isTimeSpentManual() const
{
    return m_manualTimeSpent;
}

bool RamStatus::isPublished() const
{
    return m_published;
}

void RamStatus::setPublished(bool published)
{
    if (published == m_published) return;
    m_dirty = true;
    m_published = published;
    emit dataChanged(this);
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

QDateTime RamStatus::date() const
{
    return m_date;
}

void RamStatus::setDate(const QDateTime &date)
{
    if (m_date == date) return;
    m_dirty = true;
    m_date = date;
    emit dataChanged(this);
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
    status->setGoal( other->goal() );
    status->item()->addStatus(status);
    status->setUseAutoEstimation( other->useAutoEstimation() );
    status->setComment( other->comment() );
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

