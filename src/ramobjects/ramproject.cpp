#include "ramproject.h"

RamProject::RamProject(QString shortName, QString name, QString uuid, QObject *parent):
    RamObject(shortName, name, uuid, parent)
{
    setObjectType(Project);
    _sequences = new RamObjectUberList(this);
    _assetGroups = new RamObjectUberList(this);
    _pipeline = new RamObjectList(this);
    _steps = new RamObjectList(this);
    _pipeFiles = new RamObjectList(this);
    _dbi->createProject(_shortName, _name, _uuid);

    this->setObjectName( "RamProject" );
}

RamProject::~RamProject()
{
    _dbi->removeProject(_uuid);
}

QString RamProject::folderPath() const
{
    return _folderPath;
}

void RamProject::setFolderPath(const QString &folderPath)
{
    if (folderPath == _folderPath) return;
    _dirty = true;
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
    _dirty = true;
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
    _dirty = true;
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
    _dirty = true;
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
    _dirty = true;
    _aspectRatio = aspectRatio;
    emit changed(this);
}

void RamProject::update()
{
    if (!_dirty) return;
    RamObject::update();
    QString path = _folderPath;
    if (path == "") path = "auto";
    _dbi->updateProject(_uuid, _shortName, _name, _width, _height, _framerate, path);
}

RamObjectUberList *RamProject::assetGroups() const
{
    return _assetGroups;
}

RamAssetGroup *RamProject::assetGroup(QString uuid)
{
    return qobject_cast<RamAssetGroup*>(_assetGroups->fromUuid(uuid));
}

void RamProject::createAssetGroup(QString shortName, QString name)
{
    RamAssetGroup *assetGroup = new RamAssetGroup(shortName, name, _uuid);
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

RamSequence *RamProject::sequence(QString uuid)
{
    return qobject_cast<RamSequence*>(_sequences->fromUuid(uuid));
}

void RamProject::createSequence(QString shortName, QString name)
{
    RamSequence *seq = new RamSequence(shortName, name, _uuid, "", this);
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
    RamPipeFile *pf = new RamPipeFile(shortName, _uuid, this);
    _pipeFiles->append(pf);
    return pf;
}

RamObjectList *RamProject::steps() const
{
    return _steps;
}

void RamProject::assignStep(RamStep *templateStep)
{
    RamStep *step = templateStep->createFromTemplate(_uuid);
    _steps->append(step);
}

void RamProject::createStep(QString shortName, QString name)
{
    RamStep *step = new RamStep(shortName, name, _uuid);
    _steps->append(step);
}

