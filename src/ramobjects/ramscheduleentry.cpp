#include "ramscheduleentry.h"
#include "ramproject.h"

// STATIC //

QHash<QString, RamScheduleEntry*> RamScheduleEntry::m_existingObjects = QHash<QString, RamScheduleEntry*>();

RamScheduleEntry *RamScheduleEntry::get(QString uuid)
{
    if (!checkUuid(uuid, ScheduleEntry)) return nullptr;

    RamScheduleEntry *s = m_existingObjects.value(uuid);
    if (s) return s;

    // Finally return a new instance
    return new RamScheduleEntry(uuid);
}

RamScheduleEntry *RamScheduleEntry::c(RamObject *o)
{
    //return qobject_cast<RamScheduleEntry*>(o);
    // For performance, reinterpret_cast, but be careful with the object passed!
    return reinterpret_cast<RamScheduleEntry*>(o);
}

// PUBLIC //

RamScheduleEntry::RamScheduleEntry(RamUser *user, RamStep *step, QDateTime date):
       RamObject(user->shortName(), user->name() + " | " + date.toString("yyyy-MM-dd ap"), ObjectType::ScheduleEntry, user)
{
    construct();

    QJsonObject d = data();
    d.insert("user", user->uuid());
    d.insert("step", step->uuid());
    RamProject *proj = step->project();
    if (proj) d.insert("project", proj->uuid());

    d.insert("date", date.toString("yyyy-MM-dd hh:mm:ss"));
    setData(d);

    createData();

    connectEvents();
}

RamProject *RamScheduleEntry::project() const
{
    QString projUuid = getData("project").toString();
    if (projUuid != "") return RamProject::get( projUuid );
    else return nullptr;
}

RamScheduleEntry::RamScheduleEntry(QString uuid):
    RamObject(uuid, ObjectType::ScheduleEntry)
{
    construct();

    QJsonObject d = data();
    //m_user = RamUser::get( d.value("user").toString() );
    //m_date = QDateTime::fromString( d.value("date").toString(), "yyyy-MM-dd hh:mm:ss");

    //this->setParent(m_user);

    connectEvents();
}

RamUser *RamScheduleEntry::user() const
{
    QString userUuid = getData("user").toString();
    if (userUuid == "") return nullptr;
    return RamUser::get( userUuid );
}

RamStep *RamScheduleEntry::step() const
{
    return RamStep::get( getData("step").toString() );
}

const QDateTime RamScheduleEntry::date() const
{
    QDateTime d = QDateTime::fromString( getData("date").toString(), "yyyy-MM-dd hh:mm:ss");
    return d;
}

void RamScheduleEntry::setStep(RamStep *newStep)
{
    RamStep *currentStep = step();
    if (currentStep)
    {
        disconnect(currentStep, nullptr, this, nullptr);
        disconnect(this, nullptr, currentStep, nullptr);
    }

    if (newStep)
    {
        insertData("step", newStep->uuid());
        connect(newStep, SIGNAL(removed(RamObject*)), this, SLOT(stepRemoved()));
    }
    else insertData("step", "none");
}

QString RamScheduleEntry::iconName() const
{
    RamStep *s = this->step();
    if (!s) return "";
    switch( s->type() )
    {
    case RamStep::PreProduction: return ":/icons/project";
    case RamStep::ShotProduction: return ":/icons/shot";
    case RamStep::AssetProduction: return ":/icons/asset";
    case RamStep::PostProduction: return ":/icons/film";
    }
    return ":/icons/project";
}

QVariant RamScheduleEntry::roleData(int role) const
{
    switch(role)
    {
    case Qt::DisplayRole: {
        RamStep *s = this->step();
        if (s) return this->step()->shortName();
        else return "";
    }
    case Qt::ToolTipRole: {

        RamStep *s = this->step();
        QString stepName = "";
        if (s) stepName = s->name();

        RamUser *u = this->user();
        QString userName = "";
        if (u) userName = u->name();

        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", "yyyy-MM-dd").toString();
        return this->date().toString(dateFormat) +
                "\n" + stepName +
                "\n" + userName;
    }
    case Qt::StatusTipRole: {

        RamStep *s = this->step();
        QString stepName = "";
        if (s) stepName = s->shortName();

        RamUser *u = this->user();
        QString userName = "";
        if (u) userName = u->shortName();

        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", "yyyy-MM-dd").toString();
        return this->date().toString(dateFormat) +
                " | " + stepName +
                " | " + userName;
    }
    case Qt::BackgroundRole: {
        RamStep *s = this->step();
        if (s) return QBrush(s->color());
        else return QBrush();
    }
    case Qt::EditRole: {
        RamStep *s = this->step();
        if (s) return s->uuid();
        else return "";
    }
    case SizeHint: return QSize(75,10);
    case Date: return this->date();
    case IsPM: return this->date().time().hour() >= 12;
    }
    return RamObject::roleData(role);
}

void RamScheduleEntry::stepRemoved()
{
    setStep(nullptr);
}

void RamScheduleEntry::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/calendar";
    m_editRole = Lead;
}

void RamScheduleEntry::connectEvents()
{

}
