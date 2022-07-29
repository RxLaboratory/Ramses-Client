#include "ramproject.h"

#include "ramses.h"
#include "projecteditwidget.h"

// STATIC

RamProject *RamProject::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamProject( uuid );
    return qobject_cast<RamProject*>( obj );
}

// PUBLIC //


// PROTECTED //

RamProject::RamProject(QString uuid):
    RamObject(uuid, Project)
{
    construct();

    // Populate lists

    QJsonObject d = data();

    m_sequences = RamObjectList<RamSequence*>::getObject( d.value("sequences").toString(), true);
    m_sequences->setParent(this);
    m_assetGroups = RamObjectList<RamAssetGroup*>::getObject( d.value("assetGroups").toString(), true);
    m_assetGroups->setParent(this);
    m_pipeline = RamObjectList<RamPipe*>::getObject( d.value("pipeline").toString(), true);
    m_pipeline->setParent(this);
    m_steps = RamObjectList<RamStep*>::getObject( d.value("steps").toString(), true);
    m_steps->setParent(this);
    m_pipeFiles = RamObjectList<RamPipeFile*>::getObject( d.value("pipeFiles").toString(), true);
    m_pipeFiles->setParent(this);
    m_shots = RamItemTable<RamShot*>::getObject( d.value("shots").toString(), true);
    m_shots->setParent(this);
    m_assets = RamItemTable<RamAsset*>::getObject( d.value("assets").toString(), true);
    m_assets->setParent(this);
    m_users = RamObjectList<RamUser*>::getObject( d.value("users").toString(), true);
    m_users->setParent(this);
    m_scheduleComments = RamObjectList<RamScheduleComment*>::getObject( d.value("scheduleComments").toString(), true);
    m_scheduleComments->setParent(this);
}

// PRIVATE //

void RamProject::construct()
{
    m_icon = ":/icons/project";
    m_editRole = Admin;
}
















RamProject::RamProject(QString shortName, QString name):
    RamObject(shortName, name, uuid, Ramses::instance())
{


    setObjectType(Project);


    m_dbi->createProject(m_shortName, m_name, m_uuid);

    m_deadline = QDate::currentDate().addDays(30);

    // Get path
    QSettings settings;
    settings.beginGroup("projects");
    settings.beginGroup(m_uuid);
    QString p = settings.value("path", "no-path").toString();
    settings.endGroup();
    settings.endGroup();
    if (p != "no-path") m_folderPath = p;

    this->setObjectName( "RamProject" );

    connect(m_users, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(userAssigned(QModelIndex,int,int)));
    connect(m_users, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(userUnassigned(QModelIndex,int,int)));
}



void RamProject::freezeEstimations(bool freeze, bool reCompute)
{
    m_freezeEstimations = freeze;

    // Freeze steps
    for(int i = 0; i < this->steps()->count(); i++)
    {
        RamStep *step = qobject_cast<RamStep*>( this->steps()->at(i) );
        step->freezeEstimations(freeze, reCompute);
    }

    // No need to recompute, it's triggered by the steps when unfreezing
}

QString RamProject::folderPath() const
{
    if (pathIsDefault())
    {
        QString p = m_dbFolderPath;
        if (pathIsDefault(p)) return defaultPath();
        return p;
    }

    return m_folderPath;
}

const QDate &RamProject::deadline() const
{
    return m_deadline;
}

void RamProject::setDeadline(const QDate &newDeadline)
{
    if (m_deadline == newDeadline) return;
    m_dirty = true;
    m_deadline = newDeadline;
    emit dataChanged(this);
}

void RamProject::setFolderPath(const QString &folderPath)
{
    if (folderPath == m_folderPath) return;
    m_folderPath = folderPath;

    // Store this as a local setting
    QSettings settings;
    settings.beginGroup("projects");
    settings.beginGroup(m_uuid);
    settings.setValue("path", m_folderPath);
    settings.endGroup();
    settings.endGroup();

    emit dataChanged(this);
}

void RamProject::resetDbFolderPath()
{
    setFolderPath(m_dbFolderPath);
}

QString RamProject::defaultPath() const
{
    return Ramses::instance()->path(RamObject::ProjectsFolder) + "/" + m_shortName;
}

qreal RamProject::framerate() const
{
    return m_framerate;
}

void RamProject::setFramerate(const qreal &framerate)
{
    if (framerate == m_framerate) return;
    m_dirty = true;
    m_framerate = framerate;
    emit dataChanged(this);
}

int RamProject::width() const
{
    return m_width;
}

void RamProject::setWidth(const int width, const qreal &pixelAspect)
{
    if (width == m_width) return;
    m_dirty = true;
    m_width = width;
    updateAspectRatio(pixelAspect);
}

int RamProject::height() const
{
    return m_height;
}

void RamProject::setHeight(const int height, const qreal &pixelAspect)
{
    if (height == m_height) return;
    m_dirty = true;
    m_height = height;
    updateAspectRatio(pixelAspect);
}

qreal RamProject::aspectRatio() const
{
    return m_aspectRatio;
}

void RamProject::updateAspectRatio(const qreal &pixelAspect)
{
    m_aspectRatio = qreal(m_width) / qreal(m_height) * pixelAspect;
    emit dataChanged(this);
}

void RamProject::setAspectRatio(const qreal &aspectRatio)
{
    if (aspectRatio == m_aspectRatio) return;
    m_dirty = true;
    m_aspectRatio = aspectRatio;
    emit dataChanged(this);
}

bool RamProject::pathIsDefault() const
{
    return pathIsDefault(m_folderPath);
}

bool RamProject::pathIsDefault(QString p) const
{
    return p == "" || p.toLower() == "auto";
}


void RamProject::updatePath()
{
    QString path = m_folderPath;
    if (path == "") path = "auto";
    m_dbi->updateProject(m_uuid, m_shortName, m_name, m_width, m_height, m_framerate, path, m_comment, m_deadline);
}


void RamProject::edit(bool show)
{
    if (!m_editReady)
    {
        ProjectEditWidget *w = new ProjectEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    if (show) showEdit();
}


void RamProject::computeEstimation()
{
    if (m_freezeEstimations) return;
    m_timeSpent = 0;
    m_estimation = 0;
    m_completionRatio = 0;
    m_latenessRatio = 0;
    m_assignedDays = 0;
    m_missingDays = 0;
    int numItems = 0;


    for (int i =0; i < m_steps->count(); i++)
    {
        RamStep *step = qobject_cast<RamStep*>( m_steps->at(i) );

        //Ignore pre and post procution
        if (step->type() != RamStep::ShotProduction && step->type() != RamStep::AssetProduction) continue;

        m_timeSpent += step->timeSpent();
        m_estimation += step->estimation();
        m_completionRatio += step->completionRatio();
        m_latenessRatio += step->latenessRatio();
        m_assignedDays += step->assignedDays();
        m_missingDays += step->unassignedDays();

        numItems++;
    }

    if (numItems > 0)
    {
        m_completionRatio /= numItems;
        m_latenessRatio /= numItems;
    }
    else
    {
        m_completionRatio  = 100;
        m_latenessRatio = 1;
    }

    emit completionRatioChanged(m_completionRatio);
    emit latenessRatioChanged(m_latenessRatio);
    emit timeSpentChanged(m_timeSpent);
    emit estimationChanged(m_estimation);
    emit estimationComputed(this);
}



RamProject *RamProject::projectFromName(QString nameOrShortName )
{
    return qobject_cast<RamProject*>( RamObject::objFromName(nameOrShortName, ObjectType::Project) );
}

RamObjectList *RamProject::assetGroups() const
{
    return m_assetGroups;
}

RamObjectList *RamProject::sequences() const
{
    return m_sequences;
}

RamItemTable *RamProject::shots() const
{
    return m_shots;
}

double RamProject::duration() const
{
    double duration = 0;
    for (int i = 0; i < m_sequences->count(); i++)
    {
        RamSequence *seq = qobject_cast<RamSequence*>(m_sequences->at(i));
        if (seq) duration += seq->duration();
    }
    return duration;
}

RamItemTable *RamProject::assets() const
{
    return m_assets;
}

RamObjectList *RamProject::pipeline()
{
    return m_pipeline;
}

RamPipe *RamProject::pipe(RamStep *outputStep, RamStep *inputStep)
{
    for (int i = 0; i < m_pipeline->count(); i++)
    {
        RamPipe *p = qobject_cast<RamPipe*>(m_pipeline->at(i));
        if ( p->outputStep()->is(outputStep) && p->inputStep()->is(inputStep) ) return p;
    }

    return nullptr;
}

RamObjectList *RamProject::pipeFiles()
{
    return m_pipeFiles;
}

RamObjectList *RamProject::users() const
{
    return m_users;
}

RamObjectList *RamProject::scheduleComments() const
{
    return m_scheduleComments;
}

qint64 RamProject::timeSpent() const
{
    return m_timeSpent;
}

float RamProject::estimation() const
{
    return m_estimation;
}

int RamProject::completionRatio() const
{
    return m_completionRatio;
}

float RamProject::latenessRatio() const
{
    return m_latenessRatio;
}

float RamProject::assignedDays() const
{
    return m_assignedDays;
}

float RamProject::unassignedDays() const
{
    return m_missingDays;
}

QList<float> RamProject::stats(RamUser *user)
{
    QList<float> s;
    s << 0 << 0 << 0 << 0;
    for (int i = 0; i < this->steps()->count(); i++)
    {
        RamStep *step = qobject_cast<RamStep*>( this->steps()->at(i) );
        QList<float> stepStats = step->stats(user);
        s[0] = s.at(0) + stepStats.at(0);
        s[1] = s.at(1) + stepStats.at(1);
        s[2] = s.at(2) + stepStats.at(2);
        s[3] = s.at(3) + stepStats.at(3);
    }
    return s;
}

RamObjectList *RamProject::steps() const
{
    return m_steps;
}

void RamProject::userAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *userObj = m_users->at(i);
        m_dbi->assignUser(m_uuid, userObj->uuid());
    }
    emit dataChanged(this);
}

void RamProject::userUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *userObj = m_users->at(i);
        m_dbi->unassignUser(m_uuid, userObj->uuid());
    }
    emit dataChanged(this);
}



const QString &RamProject::dbFolderPath() const
{
    return m_dbFolderPath;
}

void RamProject::setDbFolderPath(const QString &newDbFolderPath)
{
    m_dbFolderPath = newDbFolderPath;
}
