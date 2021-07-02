#include "ramproject.h"

#include "ramses.h"

RamProject::RamProject(QString shortName, QString name, QString uuid):
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(Project);
    m_sequences = new RamObjectList("SEQS", "Sequences", this);
    m_assetGroups = new RamObjectList("AGS", "Asset Groups", this);
    m_pipeline = new RamObjectList("PPLN", "Pipeline", this);
    m_steps = new RamObjectList("STPS", "Steps", this);
    m_pipeFiles = new RamObjectList("PPFLS", "Pipe files", this);
    m_shots = new RamItemTable(RamStep::ShotProduction, m_steps, "SHOTS", "Shots", this);
    m_assets = new RamItemTable(RamStep::AssetProduction, m_steps, "ASSETS", "Assets", this);
    m_users = new RamObjectList("USRS", "Users", this);

    m_dbi->createProject(m_shortName, m_name, m_uuid);

    m_deadline = QDate::currentDate().addDays(30);

    this->setObjectName( "RamProject" );

    connect(m_users, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(userAssigned(QModelIndex,int,int)));
    connect(m_users, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(userUnassigned(QModelIndex,int,int)));
}

RamProject::~RamProject()
{

}

QString RamProject::folderPath() const
{
    if (pathIsDefault())
        return defaultPath();

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
    emit changed(this);
}

void RamProject::setFolderPath(const QString &folderPath)
{
    if (folderPath == m_folderPath) return;
    m_dirty = true;
    m_folderPath = folderPath;
    emit changed(this);
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
    emit changed(this);
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
    emit changed(this);
}

void RamProject::setAspectRatio(const qreal &aspectRatio)
{
    if (aspectRatio == m_aspectRatio) return;
    m_dirty = true;
    m_aspectRatio = aspectRatio;
    emit changed(this);
}

bool RamProject::pathIsDefault() const
{
    return m_folderPath == "" || m_folderPath.toLower() == "auto";
}

void RamProject::update()
{
    if (!m_dirty) return;
    RamObject::update();
    QString path = m_folderPath;
    if (path == "") path = "auto";
    m_dbi->updateProject(m_uuid, m_shortName, m_name, m_width, m_height, m_framerate, path, m_comment, m_deadline);
}

void RamProject::removeFromDB()
{
    m_dbi->removeProject(m_uuid);
}

void RamProject::computeEstimation()
{
    m_timeSpent = 0;
    m_estimation = 0;
    m_completionRatio = 0;
    m_latenessRatio = 0;
    int numItems = 0;

    for (int i =0; i < m_steps->count(); i++)
    {
        RamStep *step = qobject_cast<RamStep*>( m_steps->at(i) );

        m_timeSpent += step->timeSpent();
        m_estimation += step->estimation();
        m_completionRatio += step->completionRatio();
        m_latenessRatio += step->latenessRatio();

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

RamProject *RamProject::project(QString uuid)
{
    return qobject_cast<RamProject*>( RamObject::obj(uuid) );
}

RamObjectList *RamProject::assetGroups() const
{
    return m_assetGroups;
}

RamObjectList *RamProject::sequences() const
{
    return m_sequences;
}

RamItemTable *RamProject::shots()
{
    return m_shots;
}

RamItemTable *RamProject::assets()
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
}

void RamProject::userUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *userObj = m_users->at(i);
        m_dbi->unassignUser(m_uuid, userObj->uuid());
    }
}
