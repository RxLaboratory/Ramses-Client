#include "statelistmanagerwidget.h"

StateListManagerWidget::StateListManagerWidget(QWidget *parent) :
    ObjectListManagerWidget(
        Ramses::instance()->states(),
        new StateEditWidget(),
        "States",
        parent)
{
    m_listEditWidget->setEditMode(ObjectListEditWidget::RemoveObjects);
    QStringList dontRemove;
    dontRemove << "TODO" << "STB" << "NO" << "WIP" << "OK";
    m_listEditWidget->setDontRemoveShortNameList(dontRemove);
}

void StateListManagerWidget::createObject()
{
    RamState *state = new RamState(
                "NEW",
                "New state");
    Ramses::instance()->states()->append(state);
    editObject(state);
}

