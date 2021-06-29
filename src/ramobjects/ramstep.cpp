#include "ramstep.h"
#include "ramproject.h"
#include "ramses.h"
#include "stepeditwidget.h"

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
    if (m_template) m_dbi->updateTemplateStep(m_uuid, m_shortName, m_name, type, m_comment, m_color);
    else m_dbi->updateStep(m_uuid, m_shortName, m_name, type, m_comment, m_color);

    if (m_orderChanged)
    {
        m_dbi->setStepOrder(m_uuid, m_order);
        m_orderChanged = false;
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
