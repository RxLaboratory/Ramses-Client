#include "rampipe.h"

RamPipe::RamPipe(RamStep *output, RamStep *input, QString uuid, QObject *parent):
    RamObject("","",uuid,parent)
{
    m_outputStep = output;
    m_inputStep = input;
    m_pipeFiles = new RamObjectList();

    m_inputConnection = connect( m_inputStep, &RamStep::removed, this, &RamObject::remove);
    m_outputConnection = connect( m_outputStep, &RamStep::removed, this, &RamObject::remove);
    m_dbi->createPipe(output->uuid(), input->uuid(), m_uuid);

    connect(m_pipeFiles, &RamObjectList::objectAdded, this, &RamPipe::pipeFileAssigned);
    connect(m_pipeFiles, &RamObjectList::objectRemoved, this, &RamPipe::pipeFileUnassigned);

    this->setObjectName( "RamPipe" );
}

RamPipe::~RamPipe()
{
    m_dbi->removePipe(m_uuid);
}

void RamPipe::update()
{
    if (!m_dirty) return;
    RamObject::update();
    m_dbi->updatePipe(m_uuid, m_inputStep->uuid(), m_outputStep->uuid());
}

RamStep *RamPipe::outputStep() const
{
    return m_outputStep;
}

void RamPipe::setOutputStep(RamStep *outputStep)
{
    if (!outputStep && !m_outputStep) return;
    if (outputStep && outputStep->is(m_outputStep)) return;
    m_dirty = true;
    disconnect( m_outputConnection );
    m_outputStep = outputStep;
    m_outputConnection = connect( m_outputStep, &RamStep::removed, this, &RamObject::remove);
    emit changed(this);
}

RamStep *RamPipe::inputStep() const
{
    return m_inputStep;
}

void RamPipe::setInputStep(RamStep *inputStep)
{
    if (!inputStep && !m_inputStep) return;
    if (inputStep && inputStep->is(m_inputStep)) return;
    m_dirty = true;
    disconnect( m_inputConnection );
    m_inputStep = inputStep;
    m_inputConnection = connect( m_inputStep, &RamStep::removed, this, &RamObject::remove);
    emit changed(this);
}

RamProject *RamPipe::project() const
{
    return m_outputStep->project();
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
    m_dbi->unassignPipeFile( m_uuid, ft->uuid());
    emit changed(this);
}

void RamPipe::pipeFileAssigned(RamObject * const ft)
{
    if (!ft) return;
    m_dbi->assignPipeFile(m_uuid, ft->uuid());
    emit changed(this);
}

