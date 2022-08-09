#include "statelistmanagerwidget.h"

#include "ramses.h"

StateListManagerWidget::StateListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget<RamState*, int>(
        Ramses::instance()->states(),
        "States",
        QIcon(":icons/state-l"),
        parent)
{
    m_listEditWidget->setEditMode(ObjectListEditWidget<RamState*, int>::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "TODO" << "STB" << "NO" << "WIP" << "OK";
    m_listEditWidget->setDontRemoveShortNameList(dontRemove);
}

RamState *StateListManagerWidget::createObject()
{
    RamState *state = new RamState(
                "NEW",
                "New state");
    Ramses::instance()->states()->append(state);
    state->edit();
    return state;
}

