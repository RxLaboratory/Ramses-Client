#include "rampipe.h"

#include "pipeeditwidget.h"

// STATIC

RamPipe *RamPipe::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamPipe( uuid );
    return qobject_cast<RamPipe*>( obj );
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

    m_pipeFiles = new RamObjectList("PPFS", "Pipe files", this);
    d.insert("pipeFiles", m_pipeFiles->uuid());
    setData(d);

    this->setParent( this->project() );

    connectEvents();
}

RamStep *RamPipe::outputStep() const
{
    return RamStep::getObject( getData("outputStep").toString() );
}

void RamPipe::setOutputStep(RamStep *outputStep)
{
    insertData("outputStep", outputStep->uuid());
}

RamStep *RamPipe::inputStep() const
{
    return RamStep::getObject( getData("inputStep").toString() );
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

// PROTECTED //

RamPipe::RamPipe(QString uuid):
    RamObject(uuid, Pipe)
{
    construct();

    QJsonObject d = data();

    m_pipeFiles = RamObjectList::getObject( d.value("pipeFiles").toString(), true);

    this->setParent( this->project() );

    connectEvents();
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
