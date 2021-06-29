#include "ramstep.h"
#include "ramproject.h"
#include "ramses.h"
#include "stepeditwidget.h"
#include "ramassetgroup.h"

RamStep::RamStep(QString shortName, QString name, QString uuid) :
    RamObject(shortName, name, uuid, Ramses::instance())
{
    setObjectType(Step);
    m_project = nullptr;
    m_template = true;
    if (m_template) m_dbi->createTemplateStep(m_shortName, m_name, m_uuid);
    init();
}

RamStep::RamStep(QString shortName, QString name, RamProject *project, QString uuid):
    RamObject(shortName, name, uuid, project)
{
    setObjectType(Step);
    m_project = project;
    m_template = false;
    m_dbi->createStep(m_shortName, m_name, m_project->uuid(), m_uuid);
    init();
}

RamStep::~RamStep()
{
    if (m_template) m_dbi->removeTemplateStep(m_uuid);
    else m_dbi->removeStep(m_uuid);
}

void RamStep::init()
{
    m_type = AssetProduction;
    m_users = new RamObjectList("", "Users", this);
    m_applications = new RamObjectList("", "Applications", this);
    connect(m_users, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(userAssigned(QModelIndex,int,int)));
    connect(m_users, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(userUnassigned(QModelIndex,int,int)));
    connect(m_applications, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(applicationAssigned(QModelIndex,int,int)));
    connect(m_applications, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(applicationUnassigned(QModelIndex,int,int)));

    this->setObjectName( "RamStep" );
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

RamObjectList *RamStep::users() const
{
    return m_users;
}

void RamStep::userAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *userObj = m_users->at(i);
        m_dbi->assignUser(m_uuid, userObj->uuid());
    }
}

void RamStep::userUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *userObj = m_users->at(i);
        m_dbi->unassignUser(m_uuid, userObj->uuid());
    }
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

QStringList RamStep::publishedTemplates() const
{
    return listFiles(TemplatesFolder, subFolderName(PublishFolder));
}

QString RamStep::templateFile(QString templateFileName) const
{
    if (templateFileName == "")
    {
        //Get the default one
        RamNameManager nm;
        nm.setProject(project()->shortName() );
        nm.setType( "G" );
        nm.setShortName( "Template" );
        nm.setStep( m_shortName );
        QDir templatesDir = QDir(path(TemplatesFolder) + "/" + subFolderName(PublishFolder));
        if(!templatesDir.exists()) return "";
        QStringList files = templatesDir.entryList(QStringList( nm.fileName() + "*" ));
        if (files.count() == 0) return "";
        templateFileName = files.at(0);
    }

    QString filePath = QDir(
                path( TemplatesFolder ) + "/" + subFolderName(PublishFolder)
                ).filePath(templateFileName);

    if (QFileInfo::exists(filePath)) return filePath;
    return "";
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
    emit estimationChanged(this);
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
    return m_latenessRatio;
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
    emit estimationChanged(this);
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
    emit estimationChanged(this);
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
    emit estimationChanged(this);
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
    emit estimationChanged(this);
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
    emit estimationChanged(this);
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
    emit estimationChanged(this);
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

    if (m_orderChanged)
    {
        m_dbi->setStepOrder(m_uuid, m_order);
        m_orderChanged = false;
    }

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

RamStep *RamStep::step(QString uuid)
{
    return qobject_cast<RamStep*>( RamObject::obj(uuid) );
}

void RamStep::edit(bool show)
{
    if (!m_editReady)
    {
        StepEditWidget *w = new StepEditWidget(this);
        setEditWidget(w);
        m_editReady = true;
    }
    showEdit(show);
}

void RamStep::computeEstimation()
{
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

    RamState *no = Ramses::instance()->noState();

    for (int i =0; i < items->count(); i++)
    {
        RamItem *item = qobject_cast<RamItem*>( items->at(i) );
        RamStatus *status = item->status(this);

        if (!status) continue;

        if (no->is(status->state())) continue;

        m_timeSpent += status->timeSpent();

        float estimation = status->estimation();
        if ( estimation < 0 ) estimation = status->autoEstimation();
        m_estimation += estimation;

        m_completionRatio += status->completionRatio();
        m_latenessRatio += status->latenessRatio();

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
