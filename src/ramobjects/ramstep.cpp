#include "ramstep.h"

#include "ramapplication.h"
#include "ramasset.h"
#include "ramassetgroup.h"
#include "ramjsonstepeditwidget.h"
#include "ramstate.h"
#include "ramstatus.h"
#include "ramnamemanager.h"
#include "ramstatustablemodel.h"
#include "ramworkingfolder.h"
#include "ramshot.h"
#include "ramses.h"

// KEYS //

const QString RamStep::KEY_EstimationMultiplyGroup = QStringLiteral("estimationMultiplyGroup");

// STATIC //

QHash<QString, RamStep*> RamStep::m_existingObjects = QHash<QString, RamStep*>();

RamStep *RamStep::get(QString uuid, bool includeRemoved)
{
    if (!checkUuid(uuid, Step)) return nullptr;

    RamStep *s = m_existingObjects.value(uuid);

    if (!s) s = new RamStep(uuid);

    if (s->isRemoved() && !includeRemoved) return nullptr;
    return s;
}

RamStep *RamStep::c(RamObject *o)
{
    //return qobject_cast<RamStep*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamStep*>(o);
}

RamStep *RamStep::createFromTemplate(RamTemplateStep *tempStep, RamProject *project)
{
    RamStep *step = new RamStep(tempStep->shortName(), tempStep->name(), project);

    // Copy data

    // populate apps list
    RamObjectModel *apps = tempStep->applications();
    for (int i = 0; i < apps->rowCount(); i++)
    {
        step->applications()->appendObject(apps->get(i)->uuid());
    }

    // Set new data
    QJsonObject tempD = tempStep->data();
    tempD.insert("project", project->uuid());
    step->setData(tempD);

    return step;
}

RamStep *RamStep::fromJson(const QJsonObject &jsonStep, RamProject *project)
{
    QString shortName = jsonStep.value(KEY_ShortName).toString();
    if (shortName == "")
        return nullptr;

    RamStep *step = new RamStep(
        shortName,
        jsonStep.value(KEY_Name).toString(shortName),
        project);

    // Set settings
    step->setComment(jsonStep.value(KEY_Comment).toString());
    step->setColor(jsonStep.value(KEY_Color).toString());
    step->setCustomSettings(jsonStep.value(KEY_CustomSettings).toString());

    step->setType(jsonStep.value(KEY_StepType).toString());
    step->setEstimationMethod(jsonStep.value(KEY_EstimationMethod).toString());
    step->setEstimationVeryEasy(jsonStep.value(KEY_EstimationVeryEasy).toDouble());
    step->setEstimationEasy(jsonStep.value(KEY_EstimationEasy).toDouble());
    step->setEstimationMedium(jsonStep.value(KEY_EstimationMedium).toDouble());
    step->setEstimationHard(jsonStep.value(KEY_EstimationHard).toDouble());
    step->setEstimationVeryHard(jsonStep.value(KEY_EstimationVeryHard).toDouble());
    step->setPublishSettings(jsonStep.value(KEY_PublishSettings).toString());

    return step;
}

// PUBLIC //

RamStep::RamStep(QString shortName, QString name, RamProject *project):
    RamTemplateStep(shortName, name, Step)
{
    construct();

    QJsonObject d = data();
    d.insert("project", project->uuid());

    setData(d);

    createData();
}

RamStep::RamStep(QString uuid):
    RamTemplateStep(uuid, Step)
{
    construct();
}

RamProject *RamStep::project() const
{
    QString projUuid = getData("project").toString();
    if (projUuid != "") return RamProject::get( projUuid );
    else return nullptr;
}

QJsonObject RamStep::toJson() const
{
    QJsonObject obj = RamObject::toJson();
    obj.insert(RamTemplateStep::KEY_StepType, getData(KEY_StepType).toString() );
    obj.insert(RamTemplateStep::KEY_EstimationMethod, getData(KEY_EstimationMethod).toString(ENUMVALUE_Shot) );

    auto group = estimationMultiplyGroup();
    QString groupUuid = ENUMVALUE_None;
    if (group)
        groupUuid = group->uuid();

    obj.insert(RamStep::KEY_EstimationMultiplyGroup, groupUuid);
    obj.insert(RamTemplateStep::KEY_EstimationVeryEasy, estimationVeryEasy() );
    obj.insert(RamTemplateStep::KEY_EstimationEasy, estimationEasy() );
    obj.insert(RamTemplateStep::KEY_EstimationMedium, estimationMedium() );
    obj.insert(RamTemplateStep::KEY_EstimationHard, estimationHard());
    obj.insert(RamTemplateStep::KEY_EstimationVeryHard, estimationVeryHard());
    obj.insert(RamTemplateStep::KEY_PublishSettings, publishSettings());

    return obj;
}

void RamStep::loadJson(const QJsonObject &obj)
{
    beginLoadJson(obj);

    setType(obj.value(RamTemplateStep::KEY_StepType).toString( RamTemplateStep::ENUMVALUE_PreProd ));
    setEstimationMethod(obj.value(RamTemplateStep::KEY_EstimationMethod).toString( RamTemplateStep::ENUMVALUE_Shot ));
    setEstimationVeryEasy(obj.value(RamTemplateStep::KEY_EstimationVeryEasy).toDouble( 0.2 ));
    setEstimationEasy(obj.value(RamTemplateStep::KEY_EstimationEasy).toDouble( 0.5 ));
    setEstimationMedium(obj.value(RamTemplateStep::KEY_EstimationMedium).toDouble( 1 ));
    setEstimationHard(obj.value(RamTemplateStep::KEY_EstimationHard).toDouble( 2 ));
    setEstimationVeryHard(obj.value(RamTemplateStep::KEY_EstimationVeryHard).toDouble( 3 ));
    setPublishSettings(obj.value(RamTemplateStep::KEY_PublishSettings).toString( ));
    QString groupUuid = obj.value(RamStep::KEY_EstimationMultiplyGroup).toString(ENUMVALUE_None);
    if (groupUuid == "")
        groupUuid = ENUMVALUE_None;
    insertData(KEY_EstimationMultiplyGroup, groupUuid );

    endLoadJson();
}

RamAssetGroup *RamStep::estimationMultiplyGroup() const
{
    return RamAssetGroup::get( getData(KEY_EstimationMultiplyGroup).toString(ENUMVALUE_None) );
}

void RamStep::setEstimationMultiplyGroup(RamObject *newEstimationMultiplyGroup)
{
    if (newEstimationMultiplyGroup)
        insertData(KEY_EstimationMultiplyGroup, newEstimationMultiplyGroup->uuid() );
    else
        insertData(KEY_EstimationMultiplyGroup, ENUMVALUE_None );
}

float RamStep::estimation(RamUser *user)
{
    RamProject *proj = project();
    if (!proj) return 0;

    Type t = type();

    if (t == PreProduction) return 0;
    if (t == PostProduction) return 0;

    RamStatusTableModel *status;
    if (t == ShotProduction) status = proj->shotStatus();
    else status = proj->assetStatus();

    if (user)
        return status->stepEstimation( m_uuid, user->uuid() );
    return status->stepEstimation( m_uuid );
}

int RamStep::completionRatio(RamUser *user)
{
    RamProject *proj = project();
    if (!proj) return 0;

    Type t = type();

    if (t == PreProduction) return 100;
    if (t == PostProduction) return 100;

    RamStatusTableModel *status;
    if (t == ShotProduction) status = proj->shotStatus();
    else status = proj->assetStatus();

    if (user)
        return status->stepCompletionRatio( m_uuid, user->uuid() );
    return status->stepCompletionRatio( m_uuid );
}

float RamStep::latenessRatio(RamUser *user)
{
    if (neededDays(user) > 0)
        return missingDays(user) / neededDays(user);
    else return 0;
}

float RamStep::assignedDays(RamUser *user)
{
    RamProject *proj = project();
    if (!proj) return 0;

    RamScheduleEntryModel *schedule = proj->scheduleEntries();

    if (!user)
        return schedule->stepCount( m_uuid ).total;
    return schedule->stepUserCount(
        user->uuid(), m_uuid
        ).total;
}

float RamStep::futureDays(RamUser *user)
{
    RamProject *proj = project();
    if (!proj) return 0;

    RamScheduleEntryModel *schedule = proj->scheduleEntries();

    if (!user)
        return schedule->stepCount( m_uuid ).future;
    return schedule->stepUserCount(
                       user->uuid(), m_uuid
                       ).future;
}

float RamStep::unassignedDays()
{
    return estimation() - assignedDays();
}

float RamStep::missingDays(RamUser *user)
{
    return neededDays(user) - futureDays(user);
}

float RamStep::daysSpent(RamUser *user)
{
    return estimation(user) * completionRatio(user) / 100;
}

float RamStep::neededDays(RamUser *user)
{
    return estimation(user) - daysSpent(user);
}

QVector<RamStep::StateCount> RamStep::stateCount(RamUser *user)
{
    QHash<RamState *, int> states;
    QVector<RamStep::StateCount> count;

    RamProject *proj = project();
    if (!proj) return count;

    const QSet<RamStatus*> status = proj->stepStatus(this);
    for(auto st: status) {
        if (!st->item())
            continue;
        RamState *s = st->state();
        if (user && !user->is(st->assignedUser()))
            continue;
        int c = states.value(s, 0);
        states.insert(s, c+1);
    }

    QHashIterator<RamState*,int> i(states);
    while(i.hasNext()) {
        i.next();
        count.append({ i.key(), i.value()} );
    }
    std::sort(count.begin(), count.end(), [] (const StateCount &a, const StateCount &b) {
        return a.state->completionRatio() < b.state->completionRatio();
    });

    return count;
}

QMap<RamStatus::Difficulty, int> RamStep::difficultyCount(RamUser *user)
{
    QMap<RamStatus::Difficulty, int> difficulty;

    RamProject *proj = project();
    if (!proj) return difficulty;

    RamState *noState = Ramses::i()->noState();

    const QSet<RamStatus*> status = proj->stepStatus(this);
    for(auto st: status) {
        if (noState->is(st->state()))
            continue;
        if (!st->item())
            continue;
        if (user && !user->is(st->assignedUser()))
            continue;
        RamStatus::Difficulty d = st->difficulty();
        int c = difficulty.value(d, 0);
        difficulty.insert(d, c+1);
    }

    return difficulty;
}

QVector<qreal> RamStep::stats(RamUser *user)
{
    float estim = estimation();
    int completion = completionRatio();

    if (!user)
    {
        return QVector<qreal>( { estim,
                                 estim * completion / 100,
                                 static_cast<float>(assignedDays()),
                                 static_cast<float>(futureDays())
                               } );
    }

    RamProject *proj = project();
    if (!proj) {
        return QVector<qreal>( 4 );
    }

    RamScheduleEntryModel *schedule = proj->scheduleEntries();
    AssignedCount uCount = schedule->stepUserCount(user->uuid(), m_uuid );

    // check completed days

    Type t = type();

    QAbstractItemModel *items;
    if (t == ShotProduction) items = proj->shots();
    else if(t == AssetProduction) items = proj->assets();
    else return QVector<qreal>( {
                                    0,
                                    0,
                                    uCount.total,
                                    uCount.future
                                } );

    float estimation = 0;
    float completedDays = 0;

    for (int i =0; i < items->rowCount(); i++)
    {
        RamAbstractItem *item = nullptr;
        QString itemUuid = items->data( items->index(i, 0), RamObject::UUID ).toString();
        if (itemUuid == "") continue;
        if (t == ShotProduction) item = RamShot::get( itemUuid );
        else item = RamAsset::get( itemUuid );

        RamStatus *status = proj->status(item, this);

        if (!status) continue;
        if (!status->state()) continue;
        if (status->state()->shortName() == "NO") continue;
        if (!status->assignedUser()) continue;
        if (!status->assignedUser()->is(user)) continue;

        float estim = 0;
        if (status->useAutoEstimation()) estim = status->estimation();
        else estim = status->goal();

        completedDays += estim * status->completionRatio() / 100.0;
        estimation += estim;
    }

    return QVector<qreal>( {
                               estimation,
                               completedDays,
                               uCount.total,
                               uCount.future
                           } );
}

void RamStep::openFile(QString filePath) const
{
    // Get the application
    for (int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->get(i) );
        if (app->open( filePath )) return;
    }

    // Try with the default app on the system
    QDesktopServices::openUrl(QUrl("file:///" + filePath));
}

QSet<RamWorkingFolder> RamStep::templateWorkingFolders() const
{
    QString templatesPath = path(TemplatesFolder);
    QDir dir(templatesPath);
    QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );

    RamNameManager nm;
    QSet<RamWorkingFolder> templateFolders;
    foreach (QString subdir, subdirs)
    {
        // check name
        if (nm.setFileName(subdir))
            templateFolders.insert(RamWorkingFolder( templatesPath + "/" + subdir ));
    }

    return templateFolders;
}

QSet<RamObject *> RamStep::inputFileTypes()
{
    QSet<RamObject *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->get(i) );
        for (int f = 0; f < app->importFileTypes()->rowCount(); f++)
        {
            RamFileType *ft = RamFileType::c( app->importFileTypes()->get(f) );
            fts << ft;
        }
        for (int f = 0; f < app->nativeFileTypes()->rowCount(); f++)
        {
            RamFileType *ft = RamFileType::c( app->nativeFileTypes()->get(f) );
            fts << ft;
        }
    }

    return fts;
}

QSet<RamObject *> RamStep::outputFileTypes()
{
    QSet<RamObject *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = RamApplication::c( m_applications->get(i) );
        for (int f = 0; f < app->exportFileTypes()->rowCount(); f++)
        {
            RamFileType *ft = RamFileType::c( app->exportFileTypes()->get(f) );
            fts << ft;
        }
        for (int f = 0; f < app->nativeFileTypes()->rowCount(); f++)
        {
            RamFileType *ft = RamFileType::c( app->nativeFileTypes()->get(f) );
            fts << ft;
        }
    }

    return fts;
}

QString RamStep::fileName() const
{
    RamProject *proj = this->project();
    if (!proj) return RamAbstractObject::fileName();
    return proj->shortName() + "_G_" + this->shortName();
}

// PUBLIC SLOTS //

void RamStep::edit(bool show)
{
    if (!ui_jsonEditWidget) {
        ui_jsonEditWidget = new RamJsonStepEditWidget(m_uuid);
        connect(ui_jsonEditWidget, &RamJsonStepEditWidget::dataEdited,
                this, &RamStep::loadJson);
        connect(ui_jsonEditWidget, &RamJsonStepEditWidget::folderOpenRequested,
                this, [this]() { revealFolder(); });
    }

    if (show) showEdit( ui_jsonEditWidget );
}

// PROTECTED //

QString RamStep::folderPath() const
{
    RamProject *proj = project();
    if (!proj) return "";

    if (type() == RamStep::PreProduction)
        return proj->path(RamObject::PreProdFolder) + "/" + proj->shortName() + "_G_" + shortName();

    else if (type() == RamStep::PostProduction)
        return proj->path(RamObject::PostProdFolder) + "/" + proj->shortName() + "_G_" + shortName();

    else
        return proj->path(RamObject::ProdFolder) + "/" + proj->shortName() + "_G_" + shortName();
}

// PRIVATE //

void RamStep::construct()
{
    m_existingObjects[m_uuid] = this;
    m_objectType = Step;
    m_editRole = RamObject::ProjectAdmin;
}
