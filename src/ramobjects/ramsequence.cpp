#include "ramsequence.h"

#include "sequenceeditwidget.h"
#include "data-models/ramitemtable.h"
#include "ramshot.h"

// STATIC //

QMap<QString, RamSequence*> RamSequence::m_existingObjects = QMap<QString, RamSequence*>();

RamSequence *RamSequence::get(QString uuid)
{
    if (!checkUuid(uuid, Sequence)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamSequence(uuid);
}

RamSequence *RamSequence::c(RamObject *o)
{
    return qobject_cast<RamSequence*>(o);
}

// PUBLIC //

RamSequence::RamSequence(QString shortName, QString name, RamProject *project):
    RamObject(shortName, name, Sequence, project)
{
    construct();
    setProject(project);
}

RamSequence::RamSequence(QString uuid):
   RamObject(uuid, Sequence)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();
    setProject( RamProject::get(projUuid) );
}

int RamSequence::shotCount() const
{
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
    return m_project;
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
    if (!ui_editWidget) setEditWidget(new SequenceEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamSequence::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/sequence";
    m_editRole = ProjectAdmin;
    m_project = nullptr;
    m_shots = new RamObjectSortFilterProxyModel(this);
    m_shots->setSingleColumn(true);
}

void RamSequence::setProject(RamProject *project)
{
    m_project = project;
    m_shots->setSourceModel( m_project->shots() );
    m_shots->setFilterUuid( m_uuid );
    this->setParent( m_project );
    insertData("project", project->uuid());
}











