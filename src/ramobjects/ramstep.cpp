#include "ramstep.h"
#include "ramproject.h"
#include "ramses.h"
#include "stepeditwidget.h"
#include "templatestepeditwidget.h"
#include "ramassetgroup.h"
#include "ramscheduleentry.h"

RamStep::RamStep(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    m_icon = ":/icons/step";
    m_editRole = Admin;

    setObjectType(Step);
    m_project = nullptr;
    m_template = true;
    if (m_template) m_dbi->createTemplateStep(m_shortName, m_name, m_uuid);
    init();
}

RamStep::RamStep(QString shortName, QString name, RamProject *project, QString uuid):
    RamObject(shortName, name, uuid, project)
{
    m_icon = ":/icons/step";
    m_editRole = ProjectAdmin;

    setObjectType(Step);
    m_project = project;
    m_template = false;
    m_dbi->createStep(m_shortName, m_name, m_project->uuid(), m_uuid);
    init();
}

RamStep::~RamStep()
{

}

void RamStep::init()
{
    m_type = AssetProduction;
    m_applications = new RamObjectList("", "Applications", this);
    connect(m_applications, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(applicationAssigned(QModelIndex,int,int)));
    connect(m_applications, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(applicationUnassigned(QModelIndex,int,int)));

    this->setObjectName( "RamStep" );
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

bool RamStep::isTemplate() const
{
    return m_template;
}

RamStep *RamStep::createFromTemplate(QString projectUuid)
{
    RamProject *project = RamProject::project( projectUuid );
    if ( !project ) return nullptr;
    return createFromTemplate(project);
}

RamStep* RamStep::createFromTemplate(RamProject *project)
{
    // Create
    RamStep *step = new RamStep(m_shortName, m_name, project);
    step->setType(m_type);
    step->setColor(m_color);
    // estimations
    step->setEstimationMethod( m_estimationMethod );
    step->setEstimationVeryEasy( m_estimationVeryEasy );
    step->setEstimationEasy( m_estimationEasy );
    step->setEstimationMedium( m_estimationMedium );
    step->setEstimationHard( m_estimationHard );
    step->setEstimationVeryHard( m_estimationVeryHard );
    // and update
    step->update();
    return step;
}

RamStep::Type RamStep::type() const
{
    return m_type;
}

void RamStep::setType(const Type &type)
{
    if (type == m_type) return;
    m_dirty = true;
    m_type = type;
    emit changed(this);
}

void RamStep::setType(QString type)
{
    if (type == "pre") setType(PreProduction);
    else if (type == "asset") setType(AssetProduction);
    else if (type == "shot") setType(ShotProduction);
    else if (type == "post") setType(PostProduction);
}

RamObjectList *RamStep::applications() const
{
    return m_applications;
}

void RamStep::openFile(QString filePath) const
{
    // Get the application
    for (int i = 0; i < m_applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( m_applications->at(i) );
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

void RamStep::applicationAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *appObj = m_applications->at(i);
        m_dbi->assignApplication(m_uuid, appObj->uuid());
    }
}

void RamStep::applicationUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *appObj = m_applications->at(i);
        m_dbi->unassignApplication(m_uuid, appObj->uuid());
    }
}

RamAssetGroup *RamStep::estimationMultiplyGroup() const
{
    return m_estimationMultiplyGroup;
}

void RamStep::setEstimationMultiplyGroup(RamAssetGroup *newEstimationMultiplyGroup)
{
    if (!newEstimationMultiplyGroup && !m_estimationMultiplyGroup) return;
    if (newEstimationMultiplyGroup)
        if (newEstimationMultiplyGroup->is(m_estimationMultiplyGroup)) return;

    m_dirty = true;
    m_estimationChanged = true;
    m_estimationMultiplyGroup = newEstimationMultiplyGroup;
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
    for (int j = 0; j < user->schedule()->count(); j++)
    {
        RamScheduleEntry *entry = qobject_cast<RamScheduleEntry*>( user->schedule()->at(j) );
        if (!entry) continue;
        if (this->is(entry->step()))
        {
            assignedHalfDays++;
            if (entry->date() > QDateTime::currentDateTime()) assignedFutureHalfDays++;
        }
    }

    // check completed days
    RamObjectList *items;
    if (m_type == ShotProduction) items = m_project->shots();
    else if(m_type == AssetProduction) items = m_project->assets();
    else return QList<float>() << 0 << 0 << assignedHalfDays / 2.0 << assignedFutureHalfDays / 2.0;

    float estimation = 0;
    float completedDays = 0;

    RamState *no = Ramses::instance()->noState();

    for (int i =0; i < items->count(); i++)
    {
        RamItem *item = qobject_cast<RamItem*>( items->at(i) );
        RamObject *stepObj = qobject_cast<RamObject*>(this);
        RamStatus *status = item->status(stepObj);

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

float RamStep::estimationVeryHard() const
{
    return m_estimationVeryHard;
}

void RamStep::setEstimationVeryHard(float newEstimationVeryHard)
{
    if (m_estimationVeryHard == newEstimationVeryHard) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationVeryHard = newEstimationVeryHard;
    emit estimationComputed(this);
}

float RamStep::estimationHard() const
{
    return m_estimationHard;
}

void RamStep::setEstimationHard(float newEstimationHard)
{
    if (m_estimationHard == newEstimationHard) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationHard = newEstimationHard;
    emit estimationComputed(this);
}

float RamStep::estimationMedium() const
{
    return m_estimationMedium;
}

void RamStep::setEstimationMedium(float newEstimationMedium)
{
    if (m_estimationMedium == newEstimationMedium) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationMedium = newEstimationMedium;
    emit estimationComputed(this);
}

float RamStep::estimationEasy() const
{
    return m_estimationEasy;
}

void RamStep::setEstimationEasy(float newEstimationEasy)
{
    if (m_estimationEasy == newEstimationEasy) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationEasy = newEstimationEasy;
    emit estimationComputed(this);
}

float RamStep::estimationVeryEasy() const
{
    return m_estimationVeryEasy;
}

void RamStep::setEstimationVeryEasy(float newEstimationVeryEasy)
{
    if (m_estimationVeryEasy == newEstimationVeryEasy) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationVeryEasy = newEstimationVeryEasy;
    emit estimationComputed(this);
}

const RamStep::EstimationMethod &RamStep::estimationMethod() const
{
    return m_estimationMethod;
}

void RamStep::setEstimationMethod(const EstimationMethod &newEstimationMethod)
{
    if (m_estimationMethod == newEstimationMethod) return;
    m_dirty = true;
    m_estimationChanged = true;
    m_estimationMethod = newEstimationMethod;
    emit estimationComputed(this);
}

const QColor &RamStep::color() const
{
    return m_color;
}

void RamStep::setColor(const QColor &newColor)
{
    if (m_color == newColor) return;
    m_dirty = true;
    m_color = newColor;
    emit changed(this);
}

QList<RamObject *> RamStep::inputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( m_applications->at(i) );
        fts.append( app->importFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

QList<RamObject *> RamStep::outputFileTypes()
{
    QList<RamObject *> fts;

    for ( int i = 0; i < m_applications->count(); i++)
    {
        RamApplication *app = qobject_cast<RamApplication*>( m_applications->at(i) );
        fts.append( app->exportFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

void RamStep::update()
{
    if (m_orderChanged)
    {
        m_dbi->setStepOrder(m_uuid, m_order);
        m_orderChanged = false;
    }

    if(!m_dirty) return;
    RamObject::update();
    QString type = "asset";
    if (m_type == PostProduction) type = "post";
    else if (m_type == PreProduction) type = "pre";
    else if (m_type == ShotProduction) type = "shot";

    if (m_template)
        m_dbi->updateTemplateStep(m_uuid, m_shortName, m_name, type, m_comment, m_color);
    else
        m_dbi->updateStep(m_uuid, m_shortName, m_name, type, m_comment, m_color);

    if (m_estimationChanged)
    {
        QString method = "shot";
        if (m_estimationMethod == RamStep::EstimatePerSecond) method = "second";
        QString multiplyGroupUuid = "";
        if (m_estimationMultiplyGroup) multiplyGroupUuid = m_estimationMultiplyGroup->uuid();

        if (m_template)
            m_dbi->setTemplateStepEstimations(m_uuid, method, m_estimationVeryEasy, m_estimationEasy, m_estimationMedium, m_estimationHard, m_estimationVeryHard);
        else
            m_dbi->setStepEstimations(m_uuid, method, m_estimationVeryEasy, m_estimationEasy, m_estimationMedium, m_estimationHard, m_estimationVeryHard, multiplyGroupUuid);
        m_estimationChanged = false;
    }
}

bool RamStep::move(int index) {
    if (!RamObject::move(index)) return false;
    m_dbi->moveStep(m_uuid, m_order);
    return true;
}

RamStep *RamStep::step(QString uuid)
{
    return qobject_cast<RamStep*>( RamObject::obj(uuid) );
}

void RamStep::edit(bool show)
{
    if (!m_editReady)
    {
        ObjectEditWidget *w;
        if (this->isTemplate())
            w = new TemplateStepEditWidget(this);
        else
            w = new StepEditWidget(this);
        setEditWidget(w);
        m_editReady = true;

    }
    if (show) showEdit();
}

void RamStep::removeFromDB()
{
    if (m_template) m_dbi->removeTemplateStep(m_uuid);
    else m_dbi->removeStep(m_uuid);
}

void RamStep::computeEstimation()
{
    if (m_freezeEstimations) return;

    if (m_type == PreProduction) return;
    if (m_type == PostProduction) return;

    RamObjectList *items;
    if (m_type == ShotProduction) items = m_project->shots();
    else items = m_project->assets();

    m_timeSpent = 0;
    m_estimation = 0;
    m_completionRatio = 0;
    m_latenessRatio = 0;
    int numItems = 0;

    float completedDays = 0;

    RamState *no = Ramses::instance()->noState();

    for (int i =0; i < items->count(); i++)
    {
        RamItem *item = qobject_cast<RamItem*>( items->at(i) );
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

    for (int i = 0; i < m_project->users()->count(); i++)
    {
        RamUser *u = qobject_cast<RamUser*>( m_project->users()->at(i) );
        if (!u) continue;

        for (int j = 0; j < u->schedule()->count(); j++)
        {
            RamScheduleEntry *entry = qobject_cast<RamScheduleEntry*>( u->schedule()->at(j) );
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

QString RamStep::folderPath() const
{
    if (m_template) return "";
    if (m_type == RamStep::PreProduction)
        return m_project->path(RamObject::PreProdFolder) + "/" + m_project->shortName() + "_G_" + m_shortName;

    else if (m_type == RamStep::PostProduction)
        return m_project->path(RamObject::PostProdFolder) + "/" + m_project->shortName() + "_G_" + m_shortName;

    else
        return m_project->path(RamObject::ProdFolder) + "/" + m_project->shortName() + "_G_" + m_shortName;
}

RamProject *RamStep::project() const
{
    return m_project;
}

void RamStep::setProject( RamProject *project )
{
    m_project = project;
}
