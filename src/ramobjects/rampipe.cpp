#include "rampipe.h"

#include "ramproject.h"

RamPipe::RamPipe(RamStep *output, RamStep *input, QString uuid):
    RamObject("PIPE","Pipe",uuid)
{
    m_outputStep = output;
    m_inputStep = input;
    m_pipeFiles = new RamObjectList("PPFS", "Pipe files", this);

    m_inputConnection = connect( m_inputStep, &RamStep::removed, this, &RamObject::remove);
    m_outputConnection = connect( m_outputStep, &RamStep::removed, this, &RamObject::remove);
    m_dbi->createPipe(output->uuid(), input->uuid(), m_uuid);

    connect(m_pipeFiles, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(pipeFileAssigned(QModelIndex,int,int)));
    connect(m_pipeFiles, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)), this, SLOT(pipeFileUnassigned(QModelIndex,int,int)));
    connect(m_pipeFiles, SIGNAL(rowsRemoved(QModelIndex,int,int)), this, SLOT(pipeFileUnassigned()));

    this->setParent( this->project() );

    this->setObjectName( "RamPipe" );
}

RamPipe::~RamPipe()
{
    m_dbi->removePipe(m_uuid);
}

QString RamPipe::name() const
{
    QStringList n;
    for (int i =0; i < m_pipeFiles->count(); i++)
    {
        n << m_pipeFiles->at(i)->name();
    }
    return n.join("\n");
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

void RamPipe::pipeFileUnassigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamObject *pfObj = m_pipeFiles->at(i);
        m_dbi->unassignPipeFile(m_uuid, pfObj->uuid());
    }
}

void RamPipe::pipeFileAssigned(const QModelIndex &parent, int first, int last)
{
    Q_UNUSED(parent)

    for (int i = first; i <= last; i++)
    {
        RamPipeFile *pf = qobject_cast<RamPipeFile*>( m_pipeFiles->at(i) );
        m_dbi->assignPipeFile(m_uuid, pf->uuid());
    }
    emit changed(this);
}

void RamPipe::pipeFileUnassigned()
{
    emit changed(this);
}

