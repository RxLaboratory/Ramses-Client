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

    setData(d);

    this->setParent(this->project());
    connectEvents();
}

RamPipe::RamPipe(QString uuid):
    RamObject(uuid, Pipe)
{
    construct();
    connectEvents();
}

RamStep *RamPipe::outputStep() const
{
    return RamStep::get( getData("outputStep").toString("none") );
}

void RamPipe::setOutputStep(RamObject *outputStep)
{
    insertData("outputStep", outputStep->uuid());
}

RamStep *RamPipe::inputStep() const
{
    return RamStep::get( getData("inputStep").toString("none") );
}

void RamPipe::setInputStep(RamObject *inputStep)
{
    insertData("inputStep", inputStep->uuid());
}

QString RamPipe::name() const
{
    if (!m_pipeFiles) return "";
    int numFiles = m_pipeFiles->rowCount();
    if (numFiles == 0) return "";

    QStringList nameList;
    for (int i = 0; i < m_pipeFiles->rowCount(); i++)
    {
        nameList << m_pipeFiles->at(i)->name();
    }
    return nameList.join("\n");
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
    getCreateLists();
}

void RamPipe::getCreateLists()
{
    QJsonObject d = data();

    QString uuid = d.value("pipeFiles").toString();
    if (uuid == "") m_pipeFiles = new RamObjectList("pipeFiles", "Files", FileType, RamObjectList::ListObject, this);
    else m_pipeFiles = RamObjectList::get( uuid, ObjectList);
    m_pipeFiles->setParent(this);
    d.insert("pipeFiles", m_pipeFiles->uuid());

    setData(d);
}

void RamPipe::connectEvents()
{
    // Set short name
    insertData("shortName", outputStep()->shortName() + "-" + inputStep()->shortName());

    connect( inputStep(), &RamStep::removed, this, &RamObject::remove);
    connect( outputStep(), &RamStep::removed, this, &RamObject::remove);

    connect(m_pipeFiles, &RamObjectList::listChanged, this, &RamPipe::pipeFileListChanged);
    connect(m_pipeFiles, &RamObjectList::dataChanged, this, &RamPipe::pipeFileListChanged);
}
