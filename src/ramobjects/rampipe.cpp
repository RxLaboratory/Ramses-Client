#include "rampipe.h"

RamPipe::RamPipe(RamStep *output, RamStep *input, QString uuid, QObject *parent):
    RamObject("","",uuid,parent)
{
    _outputStep = output;
    _inputStep = input;
    m_pipeFiles = new RamObjectList();

    _inputConnection = connect( _inputStep, &RamStep::removed, this, &RamObject::remove);
    _outputConnection = connect( _outputStep, &RamStep::removed, this, &RamObject::remove);
    _dbi->createPipe(output->uuid(), input->uuid(), _uuid);

    connect(m_pipeFiles, &RamObjectList::objectAdded, this, &RamPipe::pipeFileAssigned);
    connect(m_pipeFiles, &RamObjectList::objectRemoved, this, &RamPipe::pipeFileUnassigned);

    this->setObjectName( "RamPipe" );
}

RamPipe::~RamPipe()
{
    _dbi->removePipe(_uuid);
}

void RamPipe::update()
{
    if (!_dirty) return;
    RamObject::update();
    _dbi->updatePipe(_uuid, _inputStep->uuid(), _outputStep->uuid());
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

RamObjectList *RamPipe::pipeFiles() const
{
    return m_pipeFiles;
}

RamPipe *RamPipe::pipe(QString uuid)
{
    return qobject_cast<RamPipe*>( RamObject::obj(uuid) );
}

void RamPipe::pipeFileUnassigned(RamObject *ft)
{
    _dbi->unassignPipeFile( _uuid, ft->uuid());
    emit changed(this);
}

void RamPipe::pipeFileAssigned(RamObject * const ft)
{
    if (!ft) return;
    _dbi->assignPipeFile(_uuid, ft->uuid());
    emit changed(this);
}

