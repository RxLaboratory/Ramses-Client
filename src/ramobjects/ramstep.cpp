#include "ramstep.h"

#include "ramses.h"
#include "stepeditwidget.h"
#include "templatestepeditwidget.h"

// STATIC //

QMetaEnum const RamStep::m_stepTypeMeta = QMetaEnum::fromType<RamStep::Type>();

const QString RamStep::stepTypeName(Type type)
{
    return QString(m_stepTypeMeta.valueToKey(type));
}

RamStep::Type RamStep::stepTypeFromName(QString typeName)
{
    if (typeName == "PreProduction") return PreProduction;
    if (typeName == "AssetProduction") return AssetProduction;
    if (typeName == "ShotProduction") return ShotProduction;
    if (typeName == "PostProduction") return PostProduction;
    if (typeName == "All") return All;
}

RamStep *RamStep::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamStep( uuid );
    return qobject_cast<RamStep*>( obj );
}

// PUBLIC //

RamStep::RamStep(QString shortName, QString name) :
    RamObject(shortName, name, Step)
{
    construct();

    QJsonObject d = data();

    d.insert("template", true);
    m_applications = new RamObjectList<RamApplication*>(shortName + "-Apps", name + " | Applications", this);
    d.insert("applications", m_applications->uuid());

    setData(d);
}

RamStep::RamStep(QString shortName, QString name, RamProject *project):
    RamObject(shortName, name, Step, project)
{
    construct();
    m_project = project;

    QJsonObject d = data();

    d.insert("template", false);
    d.insert("project", project->uuid());
    m_applications = new RamObjectList<RamApplication*>(shortName + "-Apps", name + " | Applications", this);
    d.insert("applications", m_applications->uuid());

    setData(d);
}

bool RamStep::isTemplate() const
{
    return getData("template").toBool(true) || !m_project;
}

RamStep *RamStep::createFromTemplate(QString projectUuid)
{
    QJsonObject d = data();
    // Create
    RamStep *step = new RamStep(d.value("shortName").toString(), d.value("name").toString());
    // Update and assign data
    d.insert("project", projectUuid);
    d.insert("template", false);
    step->setData(d);
    return step;
}

RamStep* RamStep::createFromTemplate(RamProject *project)
{
    return createFromTemplate( project->uuid() );
}

RamProject *RamStep::project() const
{
    return m_project;
}

RamObjectList<RamApplication *> *RamStep::applications() const
{
    return m_applications;
}

RamStep::Type RamStep::type() const
{
    QString typeStr = getData("type").toString();

    if (typeStr == "pre") return PreProduction;
    else if (typeStr == "asset") return AssetProduction;
    else if (typeStr == "shot") return ShotProduction;
    else if (typeStr == "post") return PostProduction;
    else return All;
}

void RamStep::setType(const Type &type)
{
    switch(type){
    case PreProduction:
        insertData("type", "pre");
        break;
    case AssetProduction:
        insertData("type", "asset");
        break;
    case ShotProduction:
        insertData("type", "shot");
        break;
    case PostProduction:
        insertData("type", "post");
        break;
    case All:
        insertData("type", "all");
        break;
    }
}

void RamStep::setType(QString type)
{
    insertData("type", type);
}

QColor RamStep::color() const
{
    return QColor( getData("color").toString("#434343") );
}

void RamStep::setColor(const QColor &newColor)
{
    insertData("color", newColor.name());
}

QString RamStep::publishSettings() const
{
    return getData("publishSettings").toString();
}

void RamStep::setPublishSettings(const QString &newPublishSettings)
{
    insertData("publishSettings", newPublishSettings);
}

float RamStep::estimationVeryHard() const
{
    return getData("estimationVeryHard").toDouble();
}

void RamStep::setEstimationVeryHard(float newEstimationVeryHard)
{
    insertData("estimationVeryHard", newEstimationVeryHard);
}

float RamStep::estimationHard() const
{
    return getData("estimationHard").toDouble();
}

void RamStep::setEstimationHard(float newEstimationHard)
{
    insertData("estimationHard", newEstimationHard);
}

float RamStep::estimationMedium() const
{
    return getData("estimationMedium").toDouble();
}

void RamStep::setEstimationMedium(float newEstimationMedium)
{
    insertData("estimationMedium", newEstimationMedium);
}

float RamStep::estimationEasy() const
{
    return getData("estimationEasy").toDouble();
}

void RamStep::setEstimationEasy(float newEstimationEasy)
{
    insertData("estimationEasy", newEstimationEasy);
}

float RamStep::estimationVeryEasy() const
{
    return getData("estimationVeryEasy").toDouble();
}

void RamStep::setEstimationVeryEasy(float newEstimationVeryEasy)
{
    insertData("estimationVeryEasy", newEstimationVeryEasy);
}

RamStep::EstimationMethod RamStep::estimationMethod() const
{
    QString methodStr = getData("estimationMethod").toString("shot");
    if (methodStr == "shot") return EstimatePerShot;
    else return EstimatePerSecond;
}

void RamStep::setEstimationMethod(const EstimationMethod &newEstimationMethod)
{
    switch(newEstimationMethod)
    {
    case EstimatePerShot:
        insertData("estimationMethod", "shot");
        break;
    case EstimatePerSecond:
        insertData("estimationMethod", "second");
        break;
    }
}

RamAssetGroup *RamStep::estimationMultiplyGroup() const
{
    return RamAssetGroup::getObject( getData("estimationMultiplyGroup").toString("none"), true);
}

void RamStep::setEstimationMultiplyGroup(RamAssetGroup *newEstimationMultiplyGroup)
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
    RamObjectList<RamItem*> *items;
    if (type() == ShotProduction) items = m_project->shots();
    else if(type() == AssetProduction) items = m_project->assets();
    else return QList<float>() << 0 << 0 << assignedHalfDays / 2.0 << assignedFutureHalfDays / 2.0;

    float estimation = 0;
    float completedDays = 0;

    RamState *no = Ramses::instance()->noState();

    for (int i =0; i < items->rowCount(); i++)
    {
        RamItem *item = items->at(i);
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
        RamApplication *app = m_applications->at(i);
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

QList<RamFileType *> RamStep::inputFileTypes()
{
    QList<RamFileType *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = m_applications->at(i);
        fts.append( app->importFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

QList<RamFileType *> RamStep::outputFileTypes()
{
    QList<RamFileType *> fts;

    for ( int i = 0; i < m_applications->rowCount(); i++)
    {
        RamApplication *app = m_applications->at(i);
        fts.append( app->exportFileTypes()->toList() );
        fts.append( app->nativeFileTypes()->toList() );
    }

    return fts;
}

// PUBLIC SLOTS //

void RamStep::edit(bool show)
{
    if (!ui_editWidget)
    {
        if (this->isTemplate())
            setEditWidget(new TemplateStepEditWidget(this));
        else
            setEditWidget(new StepEditWidget(this));
    }

    if (show) showEdit();
}

void RamStep::computeEstimation()
{
    if (m_freezeEstimations) return;

    Type t = type();

    if (t == PreProduction) return;
    if (t == PostProduction) return;

    RamObjectList<RamItem*> *items;
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
        RamItem *item = items->at(i);
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
        RamUser *u = m_project->users()->at(i);
        if (!u) continue;

        for (int j = 0; j < u->schedule()->count(); j++)
        {
            RamScheduleEntry *entry = u->schedule()->at(j);
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

RamStep::RamStep(QString uuid):
    RamObject(uuid, Step)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();

    if (!d.value("template").toBool(true) && projUuid != "")
    {
        m_project = RamProject::getObject( projUuid, true );
    }

    m_applications = RamObjectList<RamApplication*>::getObject( d.value("applications").toString(), true );
}

QString RamStep::folderPath() const
{
    if (isTemplate()) return "";
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
    m_icon = ":/icons/step";
    m_editRole = Admin;

    m_project = nullptr;
}
