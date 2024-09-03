#include "ramscheduleentry.h"
#include "duapp/app-config.h"
#include "qjsondocument.h"
#include "ramstep.h"
#include "ramuser.h"
#include "scheduleentryeditwidget.h"

QFrame *RamScheduleEntry::ui_editWidget = nullptr;

QHash<QString, RamScheduleEntry*> RamScheduleEntry::m_existingObjects = QHash<QString, RamScheduleEntry*>();

RamScheduleEntry *RamScheduleEntry::get(const QString &uuid)
{
    if (!checkUuid(uuid, ScheduleEntry)) return nullptr;

    RamScheduleEntry *e = m_existingObjects.value(uuid);
    if (e) return e;

    // Finally return a new instance
    return new RamScheduleEntry(uuid);
}

QVector<RamScheduleEntry*> RamScheduleEntry::get(const QModelIndex &index)
{
    // Schedule entries may come as a list
    QVector<RamScheduleEntry*> entries;

    // Check the object type
    auto type = static_cast<RamObject::ObjectType>(
        index.data(RamObject::Type).toInt()
        );

    if (type != RamObject::ScheduleEntry)
        return entries;

    const QVariantList ptrList = index.data(RamObject::Pointer).toList();
    for(const auto &i: ptrList) {
        qintptr iptr = i.toULongLong();
        if (iptr)
            entries << reinterpret_cast<RamScheduleEntry*>( iptr );
    }
    return entries;
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

void RamScheduleEntry::setRow(RamObject *newRowObj)
{
    Q_ASSERT(newRowObj);
    if (!newRowObj)
        return;

    auto newRow = RamScheduleRow::c(newRowObj);
    Q_ASSERT(newRow->project());
    if (!newRow->project())
        return;

    // Disconnect previous row
    RamScheduleRow *currentRow = row();
    if (currentRow)
        disconnect(currentRow, nullptr, this, nullptr);

    insertData("row", newRow->uuid());
    insertData("project", newRow->project()->uuid());
    // If the row is removed, the entry should be removed too
    connect(newRow, &RamScheduleRow::removed, this, &RamScheduleEntry::remove);
}

RamProject *RamScheduleEntry::project() const
{
    QString uuid = getData("project").toString();

    if (uuid == "")
        return nullptr;

    return RamProject::get( uuid );
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

QString RamScheduleEntry::name() const
{
    RamStep *s = this->step();
    if (s)
        return s->shortName();

    return RamObject::name();
}

QString RamScheduleEntry::iconName() const
{
    RamStep *s = this->step();
    if (!s)
        return RamObject::iconName();;

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
}

QColor RamScheduleEntry::color() const
{
    RamStep *s = step();
    if (s) return s->color();
    return RamObject::color();
}

void RamScheduleEntry::edit(bool show)
{
    if (!ui_editWidget)
        ui_editWidget = createEditFrame(new ScheduleEntryEditWidget(this));

    if (show) showEdit( ui_editWidget );
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
