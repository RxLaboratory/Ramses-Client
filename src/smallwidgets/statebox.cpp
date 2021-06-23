#include "statebox.h"

StateBox::StateBox(QWidget *parent): QComboBox(parent)
{
    for (int i = 0; i < Ramses::instance()->states()->count(); i++)
        newState(Ramses::instance()->states()->at(i));

    /*connect(Ramses::instance()->states(), &RamObjectList::objectAdded, this, &StateBox::newState);
    connect(Ramses::instance()->states(), &RamObjectList::objectRemoved, this, &StateBox::stateRemoved);*/
    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(currentStateChanged(int)));
}

void StateBox::setCurrentState(RamState *state)
{
    if (!state) return;
    this->setCurrentText(state->shortName());
}

void StateBox::setCurrentState(QString shortName)
{
    this->setCurrentText(shortName);
}

RamState *StateBox::currentState() const
{
    return (RamState*)Ramses::instance()->states()->fromUuid( this->currentData().toString() );
}

void StateBox::newState(RamObject *state)
{
    this->addItem(state->shortName(), state->uuid());
    _stateConnections[state->uuid()] = connect(state, &RamState::changed, this, &StateBox::stateChanged);
}

void StateBox::stateRemoved(RamObject *o)
{
    for (int i = this->count() -1; i >=0; i--)
    {
        if (this->itemData(i).toString() == o->uuid())
        {
            if (_stateConnections.contains(o->uuid())) disconnect( _stateConnections.take(o->uuid()) );
            this->removeItem(i);
        }
    }
}

void StateBox::stateChanged(RamObject *o)
{
    for (int i = this->count() -1; i >=0; i--)
    {
        if (this->itemData(i).toString() == o->uuid()) this->setItemText(i, o->shortName());
    }
}

void StateBox::currentStateChanged(int i)
{
    RamState *state = (RamState*)Ramses::instance()->states()->fromUuid( this->itemData(i).toString() );
    emit currentStateChanged(state);
    if (!state) return;

    QString colorStyle = "background-color: " + state->color().name() + "; ";
    if (state->color().lightness() > 80) colorStyle += "color: #232323;  ";

    QString style = "QComboBox, QComboBox::item { ";
    style += colorStyle;
    style += "}\n";

    style += "QComboBox QAbstractItemView {";
    style += colorStyle;
    if (state->color().lightness() > 80) style += "selection-color: #232323; ";
    style += "selection-background-color: " + state->color().name() + "; ";
    style += "}\n";

    style += "QComboBox::item:selected { color: #e3e3e3; background-color: #222222; }\n";

    this->setStyleSheet(style);

    this->setToolTip(state->name());
}
