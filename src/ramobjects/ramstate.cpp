#include "ramstate.h"

#include "ramses.h"
#include "stateeditwidget.h"

// STATIC //

RamState *RamState::getObject(QString uuid, bool constructNew )
{
    RamObject *obj = RamObject::getObject(uuid);
    if (!obj && constructNew) return new RamState( uuid );
    return qobject_cast<RamState*>( obj );
}

// PUBLIC //

RamState::RamState(QString shortName, QString name) :
    RamObject(shortName, name, State, Ramses::instance())
{
    construct();
}

QColor RamState::color() const
{
    return QColor( getData("color").toString("#434343") );
}

void RamState::setColor(const QColor &color)
{
    insertData("color", color.name() );
}

int RamState::completionRatio() const
{
    return getData("completionRatio").toInt(50);
}

void RamState::setCompletionRatio(int completionRatio)
{
    insertData("completionRatio", completionRatio);
}

// PUBLIC SLOTS //

void RamState::edit(bool show)
{
    if (!ui_editWidget) setEditWidget(new StateEditWidget(this));

    if (show) showEdit();
}

// PROTECTED //

RamState::RamState(QString uuid):
    RamObject(uuid, State)
{
    construct();
}

// PRIVATE //

void RamState::construct()
{
    m_icon = ":/icons/state-l";
    m_editRole = Admin;
}
