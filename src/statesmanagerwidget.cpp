#include "statesmanagerwidget.h"

StatesManagerWidget::StatesManagerWidget(QWidget *parent): ListManagerWidget(parent)
{
    stateWidget = new StateEditWidget(this);
    this->setWidget(stateWidget);
    stateWidget->setEnabled(false);

    this->setRole(RamUser::Admin);

    foreach(RamState *state, Ramses::instance()->states()) newState(state);

    connect(Ramses::instance(), &Ramses::newState, this, &StatesManagerWidget::newState);

}

void StatesManagerWidget::currentDataChanged(QVariant data)
{
    foreach(RamState *state, Ramses::instance()->states())
    {
        if (state->uuid() == data.toString())
        {
            stateWidget->setState(state);
            stateWidget->setEnabled(true);
            return;
        }
    }
    stateWidget->setEnabled(false);
}

void StatesManagerWidget::createItem()
{
    Ramses::instance()->createState();
}

void StatesManagerWidget::removeItem(QVariant data)
{
    Ramses::instance()->removeState(data.toString());
}

void StatesManagerWidget::newState(RamState *state)
{
    if (state->uuid() != "")
    {
        QListWidgetItem *stateItem = new QListWidgetItem(state->name());
        stateItem->setData(Qt::UserRole, state->uuid());
        this->addItem(stateItem);
        connect(state, &RamState::removed, this, &StatesManagerWidget::stateRemoved);
        connect(state, &RamState::changed, this, &StatesManagerWidget::stateChanged);
    }
}

void StatesManagerWidget::stateRemoved(RamObject *state)
{
    removeData(state->uuid());
}

void StatesManagerWidget::stateChanged()
{
    RamState *state = (RamState*)QObject::sender();
    updateItem(state->uuid(), state->name());
}
