#include "statelistmanagerwidget.h"

#include "ramses.h"

StateListManagerWidget::StateListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->states(),
        "States",
        QIcon(":icons/state-l"),
        parent)
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "TODO" << "STB" << "NO" << "WIP" << "OK";
    m_listEditWidget->setDontRemoveShortNameList(dontRemove);
    m_listEditWidget->setSortMode(RamObjectList::Completion);
}

RamState *StateListManagerWidget::createObject()
{
    RamState *state = new RamState(
                "NEW",
                "New state");
    state->edit();
    return state;
}

