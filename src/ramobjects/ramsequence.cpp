#include "ramsequence.h"

#include "ramproject.h"
#include "sequenceeditwidget.h"

// STATIC //

RamSequence *RamSequence::getObject(QString uuid, bool constructNew)
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamSequence( uuid );
    return qobject_cast<RamSequence*>( obj );
}

// PUBLIC //

RamSequence::RamSequence(QString shortName, QString name, RamProject *project):
    RamObject(shortName, name, Sequence, project)
{
    construct();
    setProject(project);
}

QColor RamSequence::color() const
{
    return QColor( getData("color").toString("#434343") );
}

void RamSequence::setColor(const QColor &newColor)
{
    insertData("color", newColor.name() );
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
        quintptr iptr = m_shots->data( m_shots->index(i,0), Qt::UserRole).toULongLong();
        RamShot *shot = reinterpret_cast<RamShot*>( iptr );
        duration += shot->duration();
    }
    return duration;
}

RamProject *RamSequence::project() const
{
    return m_project;
}

// PUBLIC SLOTS //

void RamSequence::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new SequenceEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamSequence::RamSequence(QString uuid):
   RamObject(uuid, Sequence)
{
    construct();

    QJsonObject d = data();

    QString projUuid = d.value("project").toString();
    setProject( RamProject::getObject(projUuid, true) );
}

// PRIVATE //

void RamSequence::construct()
{
    m_icon = ":/icons/sequence";
    m_editRole = ProjectAdmin;
    m_project = nullptr;
    m_shots = new RamObjectFilterModel<RamShot*>(this);
}

void RamSequence::setProject(RamProject *project)
{
    m_project = project;
    m_shots->setList( m_project->shots() );
    m_shots->setFilterUuid( m_uuid );
    this->setParent( m_project );
}











