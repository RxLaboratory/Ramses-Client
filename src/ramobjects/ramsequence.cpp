#include "ramsequence.h"

#include "sequenceeditwidget.h"

// STATIC //

QFrame *RamSequence::ui_editWidget = nullptr;

QHash<QString, RamSequence*> RamSequence::m_existingObjects = QHash<QString, RamSequence*>();

RamSequence *RamSequence::get(QString uuid)
{
    if (!checkUuid(uuid, Sequence)) return nullptr;

    RamSequence *s = m_existingObjects.value(uuid);
    if (s) return s;

    // Finally return a new instance
    return new RamSequence(uuid);
}

RamSequence *RamSequence::c(RamObject *o)
{
    return qobject_cast<RamSequence*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamSequence*>(o);
}

// PUBLIC //

RamSequence::RamSequence(QString shortName, QString name, RamProject *project):
    RamObject(shortName, name, Sequence, project)
{
    construct();
    this->setParent( project );
    insertData("project", project->uuid());
    createData();
}

RamSequence::RamSequence(QString uuid):
   RamObject(uuid, Sequence)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();
    if (projUuid != "") this->setParent( RamProject::get(projUuid) );
    else invalidate();
}

DBTableModel *RamSequence::shots() const
{
    m_shots->load();
    return m_shots;
}

int RamSequence::shotCount() const
{
    m_shots->load();
    return m_shots->rowCount();
}

double RamSequence::duration() const
{
    double duration = 0;
    for (int i = 0; i < m_shots->rowCount(); i++)
    {
        int d = m_shots->data( m_shots->index(i,0), RamObject::Duration).toDouble();
        duration += d;
    }
    return duration;
}

RamProject *RamSequence::project() const
{
    QString projUuid = getData("project").toString();
    return RamProject::get(projUuid);
}

QString RamSequence::details() const
{
    QTime dur(0, 0, duration());
    return "Contains: " +
            QString::number(shotCount()) +
            " shots\n" +
            "Duration: " +
            dur.toString("mm 'mn' ss 's'");
}

QVariant RamSequence::roleData(int role) const
{
    switch(role) {
    case Duration: return this->duration();
    }
    return RamObject::roleData(role);
}

// PUBLIC SLOTS //

void RamSequence::edit(bool show)
{
    if (!ui_editWidget) ui_editWidget = createEditFrame(new SequenceEditWidget());

    if (show) showEdit( ui_editWidget );
}

// PRIVATE //

void RamSequence::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/sequence";
    m_editRole = ProjectAdmin;

    m_shots = new DBTableModel(RamObject::Shot, true, true, this);
    m_shots->addFilterValue( "sequence", this->uuid() );
}
