#include "rampipe.h"

RamPipe::RamPipe(RamStep *output, RamStep *input, QString uuid, QObject *parent):
    RamObject("","",uuid,parent)
{
    _outputStep = output;
    _inputStep = input;
    _fileType = nullptr;

    _inputConnection = connect( _inputStep, &RamStep::removed, this, &RamObject::remove);
    _outputConnection = connect( _outputStep, &RamStep::removed, this, &RamObject::remove);
    _dbi->createPipe(output->uuid(), input->uuid(), _uuid);
}

RamPipe::~RamPipe()
{
    _dbi->removePipe(_uuid);
}

void RamPipe::update()
{
    if (!_dirty) return;
    RamObject::update();
    QString fileTypeUuid = "";
    if (_fileType) fileTypeUuid = _fileType->uuid();
    _dbi->updatePipe(_uuid, _inputStep->uuid(), _outputStep->uuid(), "", fileTypeUuid);
}

RamStep *RamPipe::outputStep() const
{
    return _outputStep;
}

void RamPipe::setOutputStep(RamStep *outputStep)
{
    if (!outputStep && !_outputStep) return;
    if (outputStep && outputStep->is(_outputStep)) return;
    _dirty = true;
    disconnect( _outputConnection );
    _outputStep = outputStep;
    _outputConnection = connect( _outputStep, &RamStep::removed, this, &RamObject::remove);
    emit changed(this);
}

RamStep *RamPipe::inputStep() const
{
    return _inputStep;
}

void RamPipe::setInputStep(RamStep *inputStep)
{
    if (!inputStep && !_inputStep) return;
    if (inputStep && inputStep->is(_inputStep)) return;
    _dirty = true;
    disconnect( _inputConnection );
    _inputStep = inputStep;
    _inputConnection = connect( _inputStep, &RamStep::removed, this, &RamObject::remove);
    emit changed(this);
}

QString RamPipe::projectUuid() const
{
    return _projectUuid;
}

void RamPipe::setProjectUuid(const QString &projectUuid)
{
    if (projectUuid == _projectUuid) return;
    _projectUuid = projectUuid;
}

RamFileType *RamPipe::fileType() const
{
    return _fileType;
}

void RamPipe::setFileType(RamFileType *fileType)
{
    if (!fileType && !_fileType) return;
    if (fileType && fileType->is(_fileType) ) return;
    _dirty = true;
    disconnect(_fileTypeConnection);
    _fileType = fileType;
    if (fileType) _fileTypeConnection = connect(_fileType, &RamFileType::removed, this, &RamPipe::removeFileType);
    emit changed(this);
}

void RamPipe::removeFileType()
{
    disconnect(_fileTypeConnection);
    _fileType = nullptr;
    emit changed(this);
}

