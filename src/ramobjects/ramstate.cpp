#include "ramstate.h"

#include "stateeditwidget.h"

// STATIC //

QMap<QString, RamState*> RamState::m_existingObjects = QMap<QString, RamState*>();

RamState *RamState::get(QString uuid )
{
    if (!checkUuid(uuid, State)) return nullptr;

    if (m_existingObjects.contains(uuid)) return m_existingObjects.value(uuid);

    // Finally return a new instance
    return new RamState(uuid);
}

RamState *RamState::c(RamObject *o)
{
    return qobject_cast<RamState*>(o);
}

// PUBLIC //

RamState::RamState(QString shortName, QString name) :
    RamObject(shortName, name, State)
{
    construct();
}

RamState::RamState(QString uuid):
    RamObject(uuid, State)
{
    construct();
}

int RamState::completionRatio() const
{
    return getData("completionRatio").toInt(50);
}

void RamState::setCompletionRatio(int completionRatio)
{
    insertData("completionRatio", completionRatio);
}

QString RamState::details() const
{
    return "Completion ratio: " + QString::number(completionRatio()) + "%";
}

QVariant RamState::roleData(int role) const
{
    switch(role)
    {
    case RamAbstractObject::Completion: return this->completionRatio();
    }
    return RamObject::roleData(role);
}

// PUBLIC SLOTS //

void RamState::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new StateEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamState::construct()
{
    m_existingObjects[m_uuid] = this;
    m_icon = ":/icons/state-l";
    m_editRole = Admin;
}
