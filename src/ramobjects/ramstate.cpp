#include "ramstate.h"

#include "stateeditwidget.h"

// STATIC //

RamState *RamState::get(QString uuid )
{
    return c( RamObject::get(uuid, State) );
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

// PUBLIC SLOTS //

void RamState::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new StateEditWidget(this));

    if (show) showEdit();
}

// PRIVATE //

void RamState::construct()
{
    m_icon = ":/icons/state-l";
    m_editRole = Admin;
}
