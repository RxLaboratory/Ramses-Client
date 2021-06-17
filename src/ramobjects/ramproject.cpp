#include "ramproject.h"

RamProject::RamProject(QString shortName, QString name, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Project);
    _sequences = new RamObjectUberList("Sequences", this);
    _assetGroups = new RamObjectUberList("Asset Groups", this);
    _pipeline = new RamObjectList("PPLN", "Pipeline", "", this);
    _steps = new RamObjectList("STPS", "Steps", "", this);
    _pipeFiles = new RamObjectList("PPFLS", "Pipe files", "", this);
    m_dbi->createProject(m_shortName, m_name, m_uuid);

    this->setObjectName( "RamProject" );
}

RamProject::~RamProject()
{
    m_dbi->removeProject(m_uuid);
}

QString RamProject::folderPath() const
{
    return _folderPath;
}

void RamProject::setFolderPath(const QString &folderPath)
{
    if (folderPath == _folderPath) return;
    m_dirty = true;
    _folderPath = folderPath;
    emit changed(this);
}

qreal RamProject::framerate() const
{
    return _framerate;
}

void RamProject::setFramerate(const qreal &framerate)
{
    if (framerate == _framerate) return;
    m_dirty = true;
    _framerate = framerate;
    emit changed(this);
}

int RamProject::width() const
{
    return _width;
}

void RamProject::setWidth(const int width, const qreal &pixelAspect)
{
    if (width == _width) return;
    m_dirty = true;
    _width = width;
    updateAspectRatio(pixelAspect);
}

int RamProject::height() const
{
    return _height;
}

void RamProject::setHeight(const int height, const qreal &pixelAspect)
{
    if (height == _height) return;
    m_dirty = true;
    _height = height;
    updateAspectRatio(pixelAspect);
}

qreal RamProject::aspectRatio() const
{
    return _aspectRatio;
}

void RamProject::updateAspectRatio(const qreal &pixelAspect)
{
    _aspectRatio = qreal(_width) / qreal(_height) * pixelAspect;
    emit changed(this);
}

void RamProject::setAspectRatio(const qreal &aspectRatio)
{
    if (aspectRatio == _aspectRatio) return;
    m_dirty = true;
    _aspectRatio = aspectRatio;
    emit changed(this);
}

void RamProject::update()
{
    if (!m_dirty) return;
    RamObject::update();
    QString path = _folderPath;
    if (path == "") path = "auto";
    m_dbi->updateProject(m_uuid, m_shortName, m_name, _width, _height, _framerate, path);
}

RamProject *RamProject::project(QString uuid)
{
    return qobject_cast<RamProject*>( RamObject::obj(uuid) );
}

RamObjectUberList *RamProject::assetGroups() const
{
    return _assetGroups;
}

void RamProject::createAssetGroup(QString shortName, QString name)
{
    RamAssetGroup *assetGroup = new RamAssetGroup(shortName, this, name);
    _assetGroups->append( assetGroup );
}

void RamProject::moveAssetToGroup(RamAsset *asset, QString groupUuid)
{
    for(int i = 0; i < _assetGroups->count(); i++)
    {
        RamAssetGroup *assetGroup = qobject_cast<RamAssetGroup*>( _assetGroups->at(i) );
        if (assetGroup->uuid() == groupUuid) assetGroup->append(asset);
        else assetGroup->removeAll(asset);
    }
}

void RamProject::moveAssetToGroup(RamAsset *asset, RamAssetGroup *group)
{
    moveAssetToGroup(asset, group->uuid());
}

RamObjectUberList *RamProject::sequences() const
{
    return _sequences;
}

void RamProject::createSequence(QString shortName, QString name)
{
    RamSequence *seq = new RamSequence(shortName, this, name, "", this);
    _sequences->append(seq);
}

void RamProject::moveShotToSequence(RamShot *shot, QString sequenceUuid)
{
    for(int i = 0; i < _sequences->count(); i++)
    {
        RamSequence *sequence = qobject_cast<RamSequence*>( _sequences->at(i) );
        if (sequence->uuid() == sequenceUuid) sequence->append(shot);
        else sequence->removeAll(shot);
    }
}

void RamProject::moveShotToSequence(RamShot *shot, RamSequence *sequence)
{
    moveShotToSequence(shot, sequence->uuid());
}

RamObjectList *RamProject::pipeline()
{
    return _pipeline;
}

RamPipe *RamProject::pipe(RamStep *outputStep, RamStep *inputStep)
{
    for (int i = 0; i < _pipeline->count(); i++)
    {
        RamPipe *p = qobject_cast<RamPipe*>(_pipeline->at(i));
        if ( p->outputStep()->is(outputStep) && p->inputStep()->is(inputStep) ) return p;
    }

    return nullptr;
}

RamPipe *RamProject::createPipe(RamStep *output, RamStep *input)
{
    RamPipe *p = new RamPipe(output, input);
    _pipeline->append(p);
    return p;
}

RamObjectList *RamProject::pipeFiles()
{
    return _pipeFiles;
}

RamPipeFile *RamProject::createPipeFile(QString shortName)
{
    RamPipeFile *pf = new RamPipeFile(shortName, m_uuid, this);
    _pipeFiles->append(pf);
    return pf;
}

RamObjectList *RamProject::steps() const
{
    return _steps;
}

void RamProject::assignStep(RamStep *templateStep)
{
    RamStep *step = templateStep->createFromTemplate(m_uuid);
    _steps->append(step);
}

void RamProject::createStep(QString shortName, QString name)
{
    RamStep *step = new RamStep(shortName, name, this);
    _steps->append(step);
}

