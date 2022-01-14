#include "statelistmanagerwidget.h"

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
}

RamObject *StateListManagerWidget::createObject()
{
    RamState *state = new RamState(
                "NEW",
                "New state");
    Ramses::instance()->states()->append(state);
    state->edit();
    return state;
}

