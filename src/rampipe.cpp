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
    _projectUuid = projectUuid;
}

RamFileType *RamPipe::fileType() const
{
    return _fileType;
}

void RamPipe::setFileType(RamFileType *fileType)
{
    disconnect(_fileTypeConnection);
    _fileType = fileType;
    _fileTypeConnection = connect(_fileType, &RamFileType::removed, this, &RamPipe::removeFileType);
    emit changed(this);
}

void RamPipe::removeFileType()
{
    disconnect(_fileTypeConnection);
    _fileType = nullptr;
    emit changed(this);
}

