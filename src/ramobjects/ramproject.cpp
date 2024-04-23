#include "ramproject.h"

#include "ramscheduleentrymodel.h"
#include "ramses.h"
#include "rampipe.h"
#include "projecteditwidget.h"
#include "ramshot.h"
#include "ramstatustablemodel.h"

QFrame *RamProject::ui_editWidget = nullptr;

QHash<QString, RamProject*> RamProject::m_existingObjects = QHash<QString, RamProject*>();

RamProject *RamProject::get(QString uuid)
{
    if (!checkUuid(uuid, Project)) return nullptr;

    RamProject *p = m_existingObjects.value(uuid);
    if (p) return p;

    // Finally return a new instance
    return new RamProject(uuid);
}

RamProject *RamProject::c(RamObject *o)
{
    // return qobject_cast<RamProject*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamProject*>(o);
}

// PUBLIC //

RamProject::RamProject(QString shortName, QString name):
    RamObject(shortName, name, Project)
{
    construct();
    createData();
}

RamProject::RamProject(QString uuid):
    RamObject(uuid, Project)
{
    construct();

    QJsonObject d = data();
    loadModel(m_users, "users", d);
    loadModel(m_pipeline, "pipeline", d);
}

DBTableModel *RamProject::steps() const
{
    m_steps->load();
    return m_steps;
}

DBTableModel *RamProject::shotSteps() const
{
    m_shotSteps->load();
    return m_shotSteps;
}

DBTableModel *RamProject::assetSteps() const
{
    m_assetSteps->load();
    return m_assetSteps;
}

DBTableModel *RamProject::assetGroups() const
{
    m_assetGroups->load();
    return m_assetGroups;
}

DBTableModel *RamProject::sequences() const
{
    m_sequences->load();
    return m_sequences;
}

DBTableModel *RamProject::shots() const
{
    m_shots->load();
    return m_shots;
}

DBTableModel *RamProject::assets() const
{
    m_assets->load();
    return m_assets;
}

RamObjectModel *RamProject::pipeline() const
{
    return m_pipeline;
}

DBTableModel *RamProject::pipeFiles() const
{
    m_pipeFiles->load();
    return m_pipeFiles;
}

RamObjectModel *RamProject::users() const
{
    return m_users;
}

DBTableModel *RamProject::scheduleRows() const
{
    m_scheduleRows->load();
    return m_scheduleRows;
}

RamScheduleEntryModel *RamProject::scheduleEntries() const
{
    m_scheduleEntries->load();
    return m_scheduleEntries;
}

RamStatusTableModel *RamProject::assetStatus() const
{
    m_assetStatusTable->load();
    return m_assetStatusTable;
}

RamStatusTableModel *RamProject::shotStatus() const
{
    m_shotStatusTable->load();
    return m_shotStatusTable;
}

RamStatus *RamProject::status(RamAbstractItem *item, RamStep *step) const
{
    if (item->objectType() == RamObject::Asset)
        return assetStatus()->getStatus(item, step);
    return shotStatus()->getStatus(item, step);
}

QSet<RamStatus *> RamProject::itemStatus(RamAbstractItem *item) const
{
    if (item->objectType() == RamObject::Asset)
        return assetStatus()->getItemStatus(item->uuid());
    return shotStatus()->getItemStatus(item->uuid());
}

QSet<RamStatus *> RamProject::stepStatus(RamStep *step) const
{
    if (step->type() == RamStep::AssetProduction)
        return assetStatus()->getStepStatus(step->uuid());
    else return shotStatus()->getStepStatus(step->uuid());
}

RamState *RamProject::state(RamAbstractItem *item, RamStep *step) const
{
    RamStatus *s = status(item, step);
    if (!s) return nullptr;
    return s->state();
}

bool RamProject::hasState(RamObject *stateObj, RamAbstractItem *item, RamStep *step) const
{
    RamState *s = state(item, step);
    if (!s)
    {
        if (Ramses::instance()->noState()->is(stateObj)) return true;
        return false;
    }
    return s->is(stateObj);
}

bool RamProject::hasState(RamObject *stateObj, RamStep *step) const
{
    QSet<RamStatus *> ss = stepStatus(step);
    foreach(RamStatus *s, ss)
    {
        RamState *sta = s->state();
        if (!sta && Ramses::instance()->noState()->is(stateObj))
            return true;
        if (!sta) continue;
        if (sta->is(stateObj)) return true;
    }
    return false;
}

bool RamProject::hasState(RamObject *stateObj, RamAbstractItem *item) const
{
    QSet<RamStatus *> ss = itemStatus(item);
    foreach(RamStatus *s, ss)
    {
        RamState *sta = s->state();
        if (!sta && Ramses::instance()->noState()->is(stateObj))
            return true;
        if (!sta) continue;
        if (sta->is(stateObj)) return true;
    }
    return false;
}

RamUser *RamProject::assignedUser(RamAbstractItem *item, RamStep *step) const
{
    RamStatus *s = status(item, step);
    if (s) return s->assignedUser();
    return nullptr;
}

bool RamProject::isUserAssigned(RamObject *userObj, RamAbstractItem *item, RamStep *step) const
{
    RamUser *u = assignedUser(item, step);
    if (!u) return false;
    return u->is(userObj);
}

bool RamProject::isUserAssigned(RamObject *userObj, RamAbstractItem *item) const
{
    QSet<RamStatus *> ss = itemStatus(item);
    foreach(RamStatus *s, ss)
    {
        RamUser *u = s->assignedUser();
        if (!u) continue;
        if (u->is(userObj)) return true;
    }
    return false;
}

bool RamProject::isUserAssigned(RamObject *userObj, RamStep *step) const
{
    QSet<RamStatus *> ss = stepStatus(step);
    foreach(RamStatus *s, ss)
    {
        RamUser *u = s->assignedUser();
        if (!u) continue;
        if (u->is(userObj)) return true;
    }
    return false;
}

bool RamProject::isUnassigned(RamAbstractItem *item, RamStep *step) const
{
    RamStatus *s = status(item, step);
    if (!s) return true;
    if (!s->assignedUser()) return true;
    return false;
}

bool RamProject::isUnassigned(RamStep *step) const
{
    QSet<RamStatus *> ss = stepStatus(step);
    foreach(RamStatus *s, ss)
    {
        RamUser *u = s->assignedUser();
        if (u) return false;
    }
    return true;
}

bool RamProject::isUnassigned(RamAbstractItem *item) const
{
    QSet<RamStatus *> ss = itemStatus(item);
    foreach(RamStatus *s, ss)
    {
        RamUser *u = s->assignedUser();
        if (u) return false;
    }
    return true;
}

qreal RamProject::framerate() const
{
    return getData("framerate").toDouble(24);
}

void RamProject::setFramerate(const qreal &newFramerate)
{
    insertData("framerate", newFramerate);
}

int RamProject::width() const
{
    return getData("width").toInt(1920);
}

void RamProject::setWidth(const int width)
{
    insertData("width", width);
}

int RamProject::height() const
{
    return getData("height").toInt(1080);
}

void RamProject::setHeight(const int height)
{
    insertData("height", height);
}

qreal RamProject::aspectRatio() const
{
    QJsonObject d = data();
    qreal w = d.value("width").toDouble(1920);
    qreal h = d.value("height").toDouble(1080);
    return w / h * d.value("pixelAspectRatio").toDouble(1.0);
}

qreal RamProject::pixelAspectRatio() const
{
    return getData("pixelAspectRatio").toDouble(1.0);
}

void RamProject::setPixelAspectRatio(const qreal &aspectRatio)
{
    insertData("pixelAspectRatio", aspectRatio);
}

QDate RamProject::deadline() const
{
    return QDate::fromString( getData("deadline").toString(), "yyyy-MM-dd");
}

void RamProject::setDeadline(const QDate &newDeadline)
{
    insertData("deadline", newDeadline.toString("yyyy-MM-dd"));
}

double RamProject::duration() const
{
    m_sequences->load();
    double duration = 0;
    for (int i = 0; i < m_sequences->rowCount(); i++)
    {
        RamShot *shot = RamShot::c( m_sequences->get(i) );
        if (shot) duration += shot->duration();
    }
    return duration;
}

RamPipe *RamProject::pipe(RamStep *outputStep, RamStep *inputStep)
{
    for (int i = 0; i < m_pipeline->rowCount(); i++)
    {
        RamPipe *p = RamPipe::c( m_pipeline->get(i) );
        if (!p) continue;
        RamStep *thisOutput = p->outputStep();
        RamStep *thisInput = p->inputStep();

        if ( outputStep->is(thisOutput) && inputStep->is(thisInput) ) return p;
    }
    return nullptr;
}

float RamProject::estimation()
{
    computeEstimation();
    return m_estimation;
}

int RamProject::completionRatio()
{
    computeEstimation();
    return m_completionRatio;
}

float RamProject::latenessRatio()
{
    computeEstimation();
    return m_latenessRatio;
}

float RamProject::assignedDays()
{
    computeEstimation();
    return m_assignedDays;
}

float RamProject::unassignedDays()
{
    computeEstimation();
    return m_missingDays;
}

QVector<float> RamProject::stats(RamUser *user)
{
    computeEstimation();
    QVector<float> s(4);
    m_steps->load();
    for (int i = 0; i < m_steps->rowCount(); i++)
    {
        RamStep *step = RamStep::c(m_steps->get(i));
        if (!step) continue;
        QVector<float> stepStats = step->stats(user);
        s[0] = s.at(0) + stepStats.at(0);
        s[1] = s.at(1) + stepStats.at(1);
        s[2] = s.at(2) + stepStats.at(2);
        s[3] = s.at(3) + stepStats.at(3);
    }
    return s;
}

void RamProject::setFolderPath(const QString &newFolderPath)
{
    if (newFolderPath == folderPath()) return;

    // Store this as a local setting
    QSettings settings;
    settings.beginGroup("projects");
    settings.beginGroup(m_uuid);
    settings.setValue("path", newFolderPath);
    settings.endGroup();
    settings.endGroup();

    emit dataChanged(this);
}

void RamProject::resetDbFolderPath()
{
    setFolderPath(dbFolderPath());
}

QString RamProject::defaultPath() const
{
    return Ramses::instance()->path(RamObject::ProjectsFolder) + "/" + shortName();
}

bool RamProject::pathIsDefault(QString p) const
{
    return p == "" || p.toLower() == "auto";
}

QString RamProject::dbFolderPath() const
{
    return getData("path").toString("auto");
}

void RamProject::setDbFolderPath(const QString &newDbFolderPath)
{
    insertData("path", newDbFolderPath);
}

QString RamProject::details() const
{
    return QString::number(width()) +
            " x " +
            QString::number(height()) +
            " (" +
            QString::number(aspectRatio(),'f',2) +
            ":1)" +
            " @ " +
            QString::number(framerate(), 'f', 2) +
            "fps";
}

QStringList RamProject::filterListUuids() const
{
    // Return the list of user uuids
    QStringList userUuids;
    for (int i = 0; i < m_users->rowCount(); i++)
    {
        userUuids << m_users->get(i)->uuid();
    }
    return userUuids;
}

QString RamProject::fileName() const
{
    return this->shortName() + "_";
}

// PUBLIC SLOTS //

void RamProject::updatePath()
{
    QString path = folderPath();
    if (path == "") path = "auto";
    insertData("path", path);
}

void RamProject::edit(bool show)
{
    if (!ui_editWidget) ui_editWidget = createEditFrame(new ProjectEditWidget());

    if (show) showEdit( ui_editWidget );
}

void RamProject::computeEstimation()
{
    if (m_estimationFrozen) return;
    if (m_computingEstimation) return;
    m_computingEstimation = true;

    m_estimation = 0;
    m_completionRatio = 0;
    m_latenessRatio = 0;
    m_assignedDays = 0;
    m_missingDays = 0;
    int numItems = 0;

    for (int i =0; i < m_steps->rowCount(); i++)
    {
        RamStep *step = RamStep::c(m_steps->get(i));

        if (!step) continue;

        //Ignore pre and post procution
        if ( step->type() != RamStep::ShotProduction && step->type() != RamStep::AssetProduction) continue;

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
    emit estimationChanged(m_estimation);
    emit estimationComputed(this);

    m_computingEstimation = false;
}

void RamProject::suspendEstimations(bool frozen, bool recompute)
{
    m_estimationFrozen = frozen;
    m_scheduleEntries->suspendEstimations(frozen);
    m_assetStatusTable->suspendEstimations(frozen);
    m_shotStatusTable->suspendEstimations(frozen);
    if (!frozen && recompute) this->computeEstimation();
}

// PROTECTED //

QString RamProject::folderPath() const
{
    // Get path in settings first
    QSettings settings;
    settings.beginGroup("projects");
    settings.beginGroup(m_uuid);
    QString p = settings.value("path", "no-path").toString();
    settings.endGroup();
    settings.endGroup();

    if (p == "no-path")
    {
        // Then return the DB path
        p = dbFolderPath();
    }

    if (pathIsDefault(p))
    {
        return defaultPath();
    }
    else return p;
}

// PRIVATE //

void RamProject::construct()
{
    m_estimationFrozen = true;

    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/project";
    m_editRole = ProjectAdmin;

    m_assets = new DBTableModel(RamObject::Asset, true, true, this);
    m_assets->addFilterValue( "project", this->uuid() );

    m_shots = new DBTableModel(RamObject::Shot, true, true, this);
    m_shots->addFilterValue( "project", this->uuid() );

    m_steps = new DBTableModel(RamObject::Step, true, true, this);
    m_steps->addFilterValue( "project", this->uuid() );

    m_shotSteps = new DBTableModel(RamObject::Step, true, true, this);
    m_shotSteps->addFilterValue( "project", this->uuid() );
    m_shotSteps->addFilterValue( "type", "shot" );

    m_assetSteps = new DBTableModel(RamObject::Step, true, true, this);
    m_assetSteps->addFilterValue( "project", this->uuid() );
    m_assetSteps->addFilterValue( "type", "asset" );

    m_assetGroups = new DBTableModel(RamObject::AssetGroup, true, true, this);
    m_assetGroups->addFilterValue( "project", this->uuid() );

    m_sequences = new DBTableModel(RamObject::Sequence, true, true, this);
    m_sequences->addFilterValue( "project", this->uuid() );

    m_assetStatusTable = new RamStatusTableModel( m_assetSteps, m_assets, this);

    m_shotStatusTable = new RamStatusTableModel( m_shotSteps, m_shots, this);

    m_pipeFiles = new DBTableModel(RamObject::PipeFile, true, false, this);
    m_pipeFiles->addFilterValue( "project", this->uuid() );

    m_users = createModel(RamObject::User, "users" );

    m_pipeline = createModel(RamObject::Pipe, "pipeline" );

    m_scheduleEntries = new RamScheduleEntryModel();
    m_scheduleEntries->addFilterValue( "project", this->uuid() );

    m_scheduleRows = new DBTableModel(RamObject::ScheduleRow, true, true, this);
    m_scheduleRows->addFilterValue( "project", this->uuid() );

    connect(m_assetStatusTable, &RamStatusTableModel::estimationsChanged, this, &RamProject::computeEstimation);

    m_estimationFrozen = false;
}

