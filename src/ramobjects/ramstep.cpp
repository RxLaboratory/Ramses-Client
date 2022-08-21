#include "ramstep.h"

#include "ramapplication.h"
#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramscheduleentry.h"
#include "ramses.h"
#include "ramstatus.h"
#include "ramnamemanager.h"
#include "data-models/ramitemtable.h"
#include "ramworkingfolder.h"
#include "stepeditwidget.h"
#include "ramshot.h"

// STATIC //

QMap<QString, RamStep*> RamStep::m_existingObjects = QMap<QString, RamStep*>();

RamStep *RamStep::get(QString uuid)
{
    if (!checkUuid(uuid, Step)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamStep(uuid);
}

RamStep *RamStep::c(RamObject *o)
{
    return qobject_cast<RamStep*>(o);
}

RamStep *RamStep::createFromTemplate(RamTemplateStep *tempStep, RamProject *project)
{
    RamStep *step = new RamStep(tempStep->shortName(), tempStep->name(), project);

    // Copy data

    // populate apps list
    RamObjectList *apps = tempStep->applications();
    for (int i = 0; i < apps->rowCount(); i++)
    {
        step->applications()->append(apps->at(i));
    }

    // Set new data
    QJsonObject tempD = tempStep->data();
    tempD.insert("applications", step->applications()->uuid());
    tempD.insert("project", project->uuid());
    step->setData(tempD);

    return step;
}

// PUBLIC //

RamStep::RamStep(QString shortName, QString name, RamProject *project):
    RamTemplateStep(shortName, name, Step)
{
    construct();
    m_project = project;

    setParent(m_project);

    QJsonObject d = data();

    d.insert("project", project->uuid());

    setData(d);
}

RamStep::RamStep(QString uuid):
    RamTemplateStep(uuid, Step)
{
    construct();

    QString projUuid = getData("project").toString();
    m_project = RamProject::get( projUuid );

    setParent(m_project);
}

RamProject *RamStep::project() const
{
    return m_project;
}

RamAssetGroup *RamStep::estimationMultiplyGroup() const
{
    return RamAssetGroup::get( getData("estimationMultiplyGroup").toString("none") );
}

void RamStep::setEstimationMultiplyGroup(RamObject *newEstimationMultiplyGroup)
{
    if (newEstimationMultiplyGroup) insertData("estimationMultiplyGroup", newEstimationMultiplyGroup->uuid() );
    else insertData("estimationMultiplyGroup", "none" );

    m_estimationChanged = true;
    emit estimationComputed(this);
}

qint64 RamStep::timeSpent() const
{
    return m_timeSpent;
}

float RamStep::estimation() const
{
    return m_estimation;
}

float RamStep::completionRatio() const
{
    return m_completionRatio;
}

float RamStep::latenessRatio() const
{
    if (neededDays() > 0)
        return missingDays() / neededDays();
    else return 0;
}

float RamStep::assignedDays() const
{
    return m_scheduledHalfDays/2.0;
}

float RamStep::unassignedDays() const
{
    return m_missingDays;
}

float RamStep::missingDays() const
{
    return neededDays() - m_scheduledFutureHalfDays/2.0;
}

float RamStep::daysSpent() const
{
    return m_estimation * m_completionRatio / 100;
}

float RamStep::neededDays() const
{
    return m_estimation - daysSpent();
}

QList<float> RamStep::stats(RamUser *user)
{
    if (!user)
    {
        return QList<float>() << m_estimation
                            << m_estimation * m_completionRatio / 100
                            << m_scheduledHalfDays/2.0
                            << m_scheduledFutureHalfDays/2.0;
    }

    int assignedHalfDays = 0;
    int assignedFutureHalfDays = 0;

    // Count assigned and future days
    for (int j = 0; j < user->schedule()->rowCount(); j++)
    {
        RamScheduleEntry *entry = RamScheduleEntry::c( user->schedule()->at(j) );
        if (!entry) continue;
        if (this->is(entry->step()))
        {
            assignedHalfDays++;
            if (entry->date() > QDateTime::currentDateTime()) assignedFutureHalfDays++;
        }
    }

    // check completed days
    RamObjectList *items;
    if (type() == ShotProduction) items = m_project->shots();
    else if(type() == AssetProduction) items = m_project->assets();
    else return QList<float>() << 0 << 0 << assignedHalfDays / 2.0 << assignedFutureHalfDays / 2.0;

    float estimation = 0;
    float completedDays = 0;

    RamState *no = Ramses::instance()->noState();
    Type t = type();

    for (int i =0; i < items->rowCount(); i++)
    {
        RamAbstractItem *item;
        if (t == ShotProduction) item = RamShot::c(items->at(i));
        else item = RamAsset::c( items->at(i) );

        RamStatus *status = item->status(this);

        if (!status) continue;
        if (no->is(status->state())) continue;
        if (!status->assignedUser()) continue;
        if (!status->assignedUser()->is(user)) continue;

        float estim = 0;
        if (status->useAutoEstimation()) estim = status->estimation();
        else estim = status->goal();

        completedDays += estim * status->completionRatio() / 100.0;
        estimation += estim;
    }

    return QList<float>() << estimation << completedDays << assignedHalfDays / 2.0 << assignedFutureHalfDays / 2.0;
}

void RamStep::freezeEstimations(bool freeze, bool reCompute)
{
    m_freezeEstimations = freeze;
    if (!freeze && reCompute)
    {
        this->computeEstimation();
        this->countAssignedDays();
    }
}

void RamStep::openFile(QString filePath) const
{
    // Get the application
    for (int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->at(i) );
        if (app->open( filePath )) return;
    }

    // Try with the default app on the system
    QDesktopServices::openUrl(QUrl("file:///" + filePath));
}

QList<RamWorkingFolder> RamStep::templateWorkingFolders() const
{
    QString templatesPath = path(TemplatesFolder);
    QDir dir(templatesPath);
    QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );

    RamNameManager nm;
    QList<RamWorkingFolder> templateFolders;
    foreach (QString subdir, subdirs)
    {
        // check name
        if (nm.setFileName(subdir))
            templateFolders.append(RamWorkingFolder( templatesPath + "/" + subdir ));
    }

    return templateFolders;
}

QList<RamObject *> RamStep::inputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->at(i) );
        fts.append( app->importFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

QList<RamObject *> RamStep::outputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->at(i) );
        fts.append( app->exportFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

// PUBLIC SLOTS //

void RamStep::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new StepEditWidget(this));

    if (show) showEdit();
}

void RamStep::computeEstimation()
{
    if (m_freezeEstimations) return;

    Type t = type();

    if (t == PreProduction) return;
    if (t == PostProduction) return;

    RamItemTable *items;
    if (t == ShotProduction) items = m_project->shots();
    else items = m_project->assets();

    m_timeSpent = 0;
    m_estimation = 0;
    m_completionRatio = 0;
    m_latenessRatio = 0;
    int numItems = 0;

    float completedDays = 0;

    RamState *no = Ramses::instance()->noState();

    for (int i =0; i < items->rowCount(); i++)
    {
        RamAbstractItem *item;
        if (t == ShotProduction) item = RamShot::c( items->at(i) );
        else item = RamAsset::c( items->at(i) );
        RamStatus *status = item->status(this);

        if (!status) continue;

        if (no->is(status->state())) continue;

        m_timeSpent += status->timeSpent();

        float estimation = 0;
        if (status->useAutoEstimation()) estimation = status->estimation();
        else estimation = status->goal();

        completedDays += estimation * status->completionRatio() / 100.0;
        m_estimation += estimation;

        m_latenessRatio += status->latenessRatio();

        numItems++;
    }

    if (numItems > 0) m_latenessRatio /= numItems;
    else m_latenessRatio = 1;

    if (m_estimation > 0) m_completionRatio = completedDays / m_estimation * 100;
    else m_completionRatio = 100;

    m_completionRatio = std::min(100, (int)m_completionRatio);

    // update missing days
    m_missingDays = m_estimation - m_scheduledHalfDays/2.0;

    m_project->computeEstimation();
    emit estimationComputed(this);
}

void RamStep::countAssignedDays()
{
    if (m_freezeEstimations) return;
    m_scheduledHalfDays = 0;
    m_scheduledFutureHalfDays = 0;

    for (int i = 0; i < m_project->users()->rowCount(); i++)
    {
        RamUser *u = RamUser::c( m_project->users()->at(i) );
        if (!u) continue;

        for (int j = 0; j < u->schedule()->rowCount(); j++)
        {
            RamScheduleEntry *entry = RamScheduleEntry::c( u->schedule()->at(j) );
            if (!entry) continue;
            if (this->is(entry->step()))
            {
                m_scheduledHalfDays++;
                if (entry->date() > QDateTime::currentDateTime()) m_scheduledFutureHalfDays++;
            }
        }
    }

    // update missing
    m_missingDays = m_estimation - m_scheduledHalfDays/2.0;
    m_project->computeEstimation();
    emit estimationComputed(this);
}

// PROTECTED //

QString RamStep::folderPath() const
{
    if (type() == RamStep::PreProduction)
        return m_project->path(RamObject::PreProdFolder) + "/" + m_project->shortName() + "_G_" + shortName();

    else if (type() == RamStep::PostProduction)
        return m_project->path(RamObject::PostProdFolder) + "/" + m_project->shortName() + "_G_" + shortName();

    else
        return m_project->path(RamObject::ProdFolder) + "/" + m_project->shortName() + "_G_" + shortName();
}

// PRIVATE //

void RamStep::construct()
{
    m_existingObjects[m_uuid] = this;
    m_objectType = Step;
    m_project = nullptr;
}
