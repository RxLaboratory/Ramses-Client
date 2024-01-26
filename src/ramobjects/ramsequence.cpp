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
    if (projUuid != "") {
        this->setParent( RamProject::get(projUuid) );
    }
    else {
        invalidate();
    }
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

bool RamSequence::overrideResolution() const
{
    return getData("overrideResolution").toBool(false);
}

void RamSequence::setOverrideResolution(bool override)
{
    insertData("overrideResolution", override);
}

bool RamSequence::overrideFramerate() const
{
    return getData("overrideFramerate").toBool(false);
}

void RamSequence::setOverrideFramerate(bool override)
{
    insertData("overrideFramerate", override);
}

qreal RamSequence::framerate() const
{
    if (overrideFramerate())
        return getData("framerate").toDouble(24);
    else return project()->framerate();
}

void RamSequence::setFramerate(const qreal &newFramerate)
{
    insertData("framerate", newFramerate);
}

int RamSequence::width() const
{
    if (overrideResolution())
        return getData("width").toInt(1920);
    else return project()->width();
}

void RamSequence::setWidth(const int width)
{
    insertData("width", width);
}

int RamSequence::height() const
{
    if (overrideResolution())
        return getData("height").toInt(1080);
    else return project()->height();
}

void RamSequence::setHeight(const int height)
{
    insertData("height", height);
}

qreal RamSequence::aspectRatio() const
{
    QJsonObject d = data();
    qreal w = d.value("width").toDouble(1920);
    qreal h = d.value("height").toDouble(1080);
    return w / h * d.value("pixelAspectRatio").toDouble(1.0);
}

qreal RamSequence::pixelAspectRatio() const
{
    return getData("pixelAspectRatio").toDouble(1.0);
}

void RamSequence::setPixelAspectRatio(const qreal &aspectRatio)
{
    insertData("pixelAspectRatio", aspectRatio);
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
