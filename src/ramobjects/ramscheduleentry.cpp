#include "ramscheduleentry.h"
#include "duqf-app/app-config.h"
#include "ramstep.h"
#include "ramuser.h"

QHash<QString, RamScheduleEntry*> RamScheduleEntry::m_existingObjects = QHash<QString, RamScheduleEntry*>();

RamScheduleEntry *RamScheduleEntry::get(QString uuid)
{
    if (!checkUuid(uuid, ScheduleEntry)) return nullptr;

    RamScheduleEntry *e = m_existingObjects.value(uuid);
    if (e) return e;

    // Finally return a new instance
    return new RamScheduleEntry(uuid);
}

RamScheduleEntry *RamScheduleEntry::c(RamObject *o, bool fast)
{
    // For performance, reinterpret_cast, but be careful with the object passed!
    if (fast)
        return reinterpret_cast<RamScheduleEntry*>(o);
    // Otherwise use the safer qobject_cast
    return qobject_cast<RamScheduleEntry*>(o);
}

bool RamScheduleEntry::validateData(const QString &data)
{
    // Make sure the entry has a row
    QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject obj = doc.object();
    return obj.contains("row");
}

RamScheduleEntry::RamScheduleEntry(const QString &name, const QDate &date, RamScheduleRow *row):
    RamObject(date.toString(DATE_DATA_FORMAT), name, ScheduleEntry, row)
{
    Q_ASSERT(row);

    construct();

    QJsonObject d = data();
    d.insert("row", row->uuid());
    d.insert("date", date.toString(DATE_DATA_FORMAT));

    RamProject *proj = row->project();
    if (proj) d.insert("project", proj->uuid());

    setData(d);

    createData();

    // If the row is removed, the entry should be removed too
    connect(row, &RamScheduleRow::removed, this, &RamScheduleEntry::remove);
}

QDate RamScheduleEntry::date() const
{
    return QDate::fromString( getData("date").toString(), DATE_DATA_FORMAT);
}

void RamScheduleEntry::setDate(const QDate &date)
{
    insertData("date", date.toString(DATE_DATA_FORMAT));
}

RamScheduleRow *RamScheduleEntry::row() const
{
    QString rowUuid = getData("row").toString();
    if (rowUuid == "")
        return nullptr;
    return RamScheduleRow::get(rowUuid);
}

void RamScheduleEntry::setRow(RamScheduleRow *newRow)
{
    Q_ASSERT(newRow);

    if (!newRow)
        return;

    // Disconnect previous row
    RamScheduleRow *currentRow = row();
    if (currentRow)
        disconnect(currentRow, nullptr, this, nullptr);

    insertData("row", newRow->uuid());
    // If the row is removed, the entry should be removed too
    connect(newRow, &RamScheduleRow::removed, this, &RamScheduleEntry::remove);
}

RamStep *RamScheduleEntry::step() const
{
    QString stepUuid = getData("step").toString();

    if (stepUuid == "")
        return nullptr;

    return RamStep::get( stepUuid );
}

void RamScheduleEntry::setStep(RamObject *newStep)
{
    // Disconnect previous step
    RamStep *currentStep = step();
    if (currentStep)
    {
        disconnect(currentStep, nullptr, this, nullptr);
    }


    if (newStep) {
        insertData("step", newStep->uuid());
        // When the step is removed, the entry must be removed too
        connect(newStep, &RamObject::removed, this, &RamScheduleEntry::remove);
    }
    else {
        insertData("step", "");
    }
}

QString RamScheduleEntry::iconName() const
{
    RamStep *s = this->step();
    if (!s)
        return "";

    switch( s->type() )
    {
    case RamStep::PreProduction:
        return ":/icons/project";
    case RamStep::ShotProduction:
        return ":/icons/shot";
    case RamStep::AssetProduction:
        return ":/icons/asset";
    case RamStep::PostProduction:
        return ":/icons/film";
    }

    return ":/icons/project";
}

QColor RamScheduleEntry::color() const
{
    RamStep *s = step();
    if (s) return s->color();
    return RamObject::color();
}

QVariant RamScheduleEntry::roleData(int role) const
{
    switch(role)
    {
    case Qt::DisplayRole: { // If there's a step, use the step name
        RamStep *s = this->step();
        if (s) return this->step()->shortName();
        return "# " + this->name() + "\n\n" + this->comment();
    }
    case Qt::ToolTipRole: {

        RamStep *s = this->step();
        QString stepName = "";
        if (s) stepName = s->name();

        RamUser *u = this->row()->user();
        QString userName = "";
        if (u) userName = u->name();

        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", DATE_DATA_FORMAT).toString();
        return this->date().toString(dateFormat) +
               "\n" + stepName +
               "\n" + userName +
               "\n" + this->comment();
    }
    case Qt::StatusTipRole: {

        RamStep *s = this->step();
        QString stepName = "";
        if (s) stepName = s->shortName();

        RamUser *u = this->row()->user();
        QString userName = "";
        if (u) userName = u->shortName();

        QSettings settings;
        QString dateFormat = settings.value("appearance/dateFormat", DATE_DATA_FORMAT).toString();
        return this->date().toString(dateFormat) +
               " | " + stepName +
               " | " + userName +
               " | " + this->comment();
    }
    case Qt::BackgroundRole: {
        return QBrush(this->color());
    }
    case SizeHint: return QSize(75,10);
    case Date: return this->date();
    }
    return RamObject::roleData(role);
}

RamScheduleEntry::RamScheduleEntry(const QString &uuid):
    RamObject(uuid, ScheduleEntry)
{
    construct();
    this->setParent(this->row());
}

void RamScheduleEntry::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/calendar";
    m_editRole = Lead;
}
