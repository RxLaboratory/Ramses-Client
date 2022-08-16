#include "rampipe.h"

#include "pipeeditwidget.h"

RamPipe *RamPipe::get(QString uuid)
{
    return c( RamObject::get(uuid, Pipe) );
}

RamPipe *RamPipe::c(RamObject *o)
{
    return qobject_cast<RamPipe*>(o);
}

// PUBLIC //

RamPipe::RamPipe(RamStep *output, RamStep *input):
    RamObject("PIPE","Pipe", Pipe)
{
    construct();

    QJsonObject d = data();

    d.insert("outputStep", output->uuid());
    d.insert("inputStep", input->uuid());

    m_pipeFiles = new RamObjectList("PPFS", "Pipe files", PipeFile, RamObjectList::ListObject, this);
    d.insert("pipeFiles", m_pipeFiles->uuid());
    setData(d);

    this->setParent( this->project() );

    connectEvents();
}

RamPipe::RamPipe(QString uuid):
    RamObject(uuid, Pipe)
{
    construct();

    QJsonObject d = data();

    m_pipeFiles = RamObjectList::get( d.value("pipeFiles").toString(), ObjectList);

    this->setParent( this->project() );

    connectEvents();
}

RamStep *RamPipe::outputStep() const
{
    return RamStep::get( getData("outputStep").toString() );
}

void RamPipe::setOutputStep(RamStep *outputStep)
{
    insertData("outputStep", outputStep->uuid());
}

RamStep *RamPipe::inputStep() const
{
    return RamStep::get( getData("inputStep").toString() );
}

void RamPipe::setInputStep(RamStep *inputStep)
{
    insertData("inputStep", inputStep->uuid());
}

RamProject *RamPipe::project() const
{
    return outputStep()->project();
}

RamObjectList *RamPipe::pipeFiles() const
{
    return m_pipeFiles;
}

// PUBLIC SLOTS //

void RamPipe::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new PipeEditWidget(this));

    if (show) showEdit();
}

// PRIVATE SLOTS //

void RamPipe::pipeFileListChanged()
{
    QStringList n;
    for (int i =0; i < m_pipeFiles->rowCount(); i++)
    {
        n << m_pipeFiles->at(i)->name();
    }
    setName(n.join("\n"));
}

// PRIVATE //

void RamPipe::construct()
{
    m_icon = ":/icons/connection";
    m_editRole = ProjectAdmin;
}

void RamPipe::connectEvents()
{
    // Set short name
    insertData("shortName", outputStep()->shortName() + "-" + inputStep()->shortName());

    connect( inputStep(), &RamStep::removed, this, &RamObject::remove);
    connect( outputStep(), &RamStep::removed, this, &RamObject::remove);

    connect(m_pipeFiles, SIGNAL(listChanged()), this, SLOT(pipeFileListChanged()));
    connect(m_pipeFiles, &RamObjectList::dataChanged, this, &RamPipe::pipeFileListChanged);
}
