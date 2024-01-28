#include "statemanagerwidget.h"

#include "ramses.h"
#include "duqf-widgets/duicon.h"

StateManagerWidget::StateManagerWidget(QWidget *parent) :
    ObjectManagerWidget(
        Ramses::instance()->states(),
        "States",
        DuIcon(":icons/state-l"),
        parent)
{
    ui_listWidget->setEditMode(ObjectListWidget::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "TODO" << "STB" << "NO" << "WIP" << "OK";
    ui_listWidget->setDontRemoveShortNameList(dontRemove);
    ui_listWidget->setSortMode(RamObject::Completion);
}

RamState *StateManagerWidget::createObject()
{
    RamState *state = new RamState(
                "NEW",
                "New state");
    state->edit();
    return state;
}

