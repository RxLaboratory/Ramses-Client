#include "ramproject.h"

#include "ramsequence.h"
#include "ramses.h"
#include "rampipe.h"
#include "projecteditwidget.h"

RamProject *RamProject::get(QString uuid)
{
    return c( RamObject::get(uuid, Project) );
}

RamProject *RamProject::c(RamObject *o)
{
    return qobject_cast<RamProject*>(o);
}

// PUBLIC //

RamProject::RamProject(QString shortName, QString name):
    RamObject(shortName, name, Project)
{
    construct();
}

RamProject::RamProject(QString uuid):
    RamObject(uuid, Project)
{
    construct();
}

RamObjectList *RamProject::steps() const
{
    return m_steps;
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

RamItemTable *RamProject::assets() const
{
    return m_assets;
}

RamObjectList *RamProject::pipeline() const
{
    return m_pipeline;
}

RamObjectList *RamProject::pipeFiles() const
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

qreal RamProject::framerate() const
{
    return getData("framerate").toDouble();
}

void RamProject::setFramerate(const qreal &newFramerate)
{
    insertData("framerate", newFramerate);
}

int RamProject::width() const
{
    return getData("width").toInt(1920);
}

void RamProject::setWidth(const int width, const qreal &pixelAspect)
{
    insertData("width", width);
    updateAspectRatio(pixelAspect);
}

int RamProject::height() const
{
    return getData("height").toInt(1080);
}

void RamProject::setHeight(const int height, const qreal &pixelAspect)
{
    insertData("height", height);
    updateAspectRatio(pixelAspect);
}

qreal RamProject::aspectRatio() const
{
    QJsonObject d = data();
    double w = d.value("width").toDouble(1920);
    double h = d.value("height").toDouble(1080);
    return d.value("aspectRatio").toDouble(w/h);
}

void RamProject::updateAspectRatio(const qreal &pixelAspect)
{
    QJsonObject d = data();
    qreal w = d.value("width").toDouble(1920);
    qreal h = d.value("height").toDouble(1080);
    qreal r = w / h * pixelAspect;
    setAspectRatio(r);
}

void RamProject::setAspectRatio(const qreal &aspectRatio)
{
    insertData("aspectRatio", aspectRatio);
}

QDate RamProject::deadline() const
{
    return QDate::fromString( getData("deadline").toString(), "yyyy-MM-dd");
}

void RamProject::setDeadline(const QDate &newDeadline)
{
    insertData("deadline", newDeadline.toString("yyy-MM-dd"));
}

double RamProject::duration() const
{
    double duration = 0;
    for (int i = 0; i < m_sequences->rowCount(); i++)
    {
        RamShot *shot = RamShot::c( m_sequences->at(i) );
        duration += shot->duration();
    }
    return duration;
}

RamPipe *RamProject::pipe(RamStep *outputStep, RamStep *inputStep)
{
    for (int i = 0; i < m_pipeline->rowCount(); i++)
    {
        RamPipe *p = RamPipe::c( m_pipeline->at(i) );
        if ( p->outputStep()->is(outputStep) && p->inputStep()->is(inputStep) ) return p;
    }
    return nullptr;
}

void RamProject::freezeEstimations(bool freeze, bool reCompute)
{
    m_freezeEstimations = freeze;

    // Freeze steps
    for(int i = 0; i < m_steps->rowCount(); i++)
    {
        RamStep *step = RamStep::c(m_steps->at(i));
        step->freezeEstimations(freeze, reCompute);
    }

    // No need to recompute, it's triggered by the steps when unfreezing
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
    for (int i = 0; i < m_steps->rowCount(); i++)
    {
        RamStep *step = RamStep::c(m_steps->at(i));
        QList<float> stepStats = step->stats(user);
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

    emit dataChanged(this, data());
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

// PUBLIC SLOTS //

void RamProject::updatePath()
{
    QString path = folderPath();
    if (path == "") path = "auto";
    insertData("path", path);
}

void RamProject::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new ProjectEditWidget(this));

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


    for (int i =0; i < m_steps->rowCount(); i++)
    {
        RamStep *step = RamStep::c(m_steps->at(i));

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
    m_icon = ":/icons/project";
    m_editRole = Admin;

    getCreateLists();
}

void RamProject::getCreateLists()
{
    QJsonObject d = data();

    QString uuid = d.value("sequences").toString();
    if (uuid == "") m_sequences = new RamObjectList("sqnc", "Sequences", Sequence, RamObjectList::ListObject, this);
    else m_sequences = RamObjectList::get( uuid, ObjectList);
    m_sequences->setParent(this);
    d.insert("sequences", m_sequences->uuid());

    uuid = d.value("assetGroups").toString();
    if (uuid == "") m_assetGroups = new RamObjectList("asstgrp", "Asset groups", AssetGroup, RamObjectList::ListObject, this);
    else m_assetGroups = RamObjectList::get( uuid, ObjectList);
    m_assetGroups->setParent(this);
    d.insert("assetGroups", m_assetGroups->uuid());

    uuid = d.value("pipeline").toString();
    if (uuid == "") m_pipeline = new RamObjectList("ppln", "Pipeline", Pipe, RamObjectList::ListObject, this);
    else m_pipeline = RamObjectList::get( uuid, ObjectList);
    m_pipeline->setParent(this);
    d.insert("pipeline", m_pipeline->uuid());

    uuid = d.value("steps").toString();
    if (uuid == "") m_steps = new RamObjectList("stp", "Steps", Step, RamObjectList::ListObject, this);
    else m_steps = RamObjectList::get( uuid, ObjectList);
    m_steps->setParent(this);
    d.insert("steps", m_steps->uuid());

    uuid = d.value("pipeFiles").toString();
    if (uuid == "") m_pipeFiles = new RamObjectList("ppfl", "Pipe files", PipeFile, RamObjectList::ListObject, this);
    else m_pipeFiles = RamObjectList::get( uuid, ObjectList);
    m_pipeFiles->setParent(this);
    d.insert("pipeFiles", m_pipeFiles->uuid());

    uuid = d.value("shots").toString();
    if (uuid == "") m_shots = new RamItemTable("sht", "Shots", Shot, this);
    else m_shots = RamItemTable::get( uuid );
    m_shots->setParent(this);
    d.insert("shots", m_shots->uuid());

    uuid = d.value("assets").toString();
    if (uuid == "") m_assets = new RamItemTable("asst", "Assets", Asset, this);
    else m_assets = RamItemTable::get( uuid );
    m_assets->setParent(this);
    d.insert("assets", m_assets->uuid());

    uuid = d.value("users").toString();
    if (uuid == "") m_users = new RamObjectList("usr", "Users", User, RamObjectList::ListObject, this);
    else m_users = RamObjectList::get( uuid, ObjectList);
    m_users->setParent(this);
    d.insert("users", m_users->uuid());

    uuid = d.value("users").toString();
    if (uuid == "") m_scheduleComments = new RamObjectList("schdlcmmnt", "Schedule comments", ScheduleComment, RamObjectList::ListObject, this);
    else m_scheduleComments = RamObjectList::get( uuid, ObjectList);
    m_scheduleComments->setParent(this);
    d.insert("scheduleComments", m_scheduleComments->uuid());

    setData(d);
}
