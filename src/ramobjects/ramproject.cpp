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

    m_dbi->createProject(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamProject" );
}

RamProject::~RamProject()
{
    m_dbi->removeProject(m_uuid);
}

QString RamProject::folderPath() const
{
    return m_folderPath;
}

void RamProject::setFolderPath(const QString &folderPath)
{
    if (folderPath == m_folderPath) return;
    m_dirty = true;
    m_folderPath = folderPath;
    emit changed(this);
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

void RamProject::update()
{
    if (!m_dirty) return;
    RamObject::update();
    QString path = m_folderPath;
    if (path == "") path = "auto";
    m_dbi->updateProject(m_uuid, m_shortName, m_name, m_width, m_height, m_framerate, path, m_comment);
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

RamObjectList *RamProject::steps() const
{
    return m_steps;
}
