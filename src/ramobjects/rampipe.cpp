#include "rampipe.h"

#include "pipeeditwidget.h"
#include "ramstep.h"

QMap<QString, RamPipe*> RamPipe::m_existingObjects = QMap<QString, RamPipe*>();

RamPipe *RamPipe::get(QString uuid)
{
    if (!checkUuid(uuid, Pipe)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamPipe(uuid);
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
    loadModel(m_pipeFiles, "pipeFiles");
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
        nameList << m_pipeFiles->get(i)->name();
    }
    return nameList.join("\n");
}

RamProject *RamPipe::project() const
{
    return outputStep()->project();
}

RamObjectModel *RamPipe::pipeFiles() const
{
    return m_pipeFiles;
}

// PUBLIC SLOTS //

void RamPipe::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new PipeEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamPipe::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/connection";
    m_editRole = ProjectAdmin;

    m_pipeFiles = createModel(RamObject::PipeFile, "pipeFiles");
}

void RamPipe::connectEvents()
{
    // Set short name
    insertData("shortName", outputStep()->shortName() + "-" + inputStep()->shortName());

    connect( inputStep(), &RamStep::removed, this, &RamObject::remove);
    connect( outputStep(), &RamStep::removed, this, &RamObject::remove);
}
