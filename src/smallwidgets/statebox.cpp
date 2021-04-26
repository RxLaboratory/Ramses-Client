#include "statebox.h"

StateBox::StateBox(QWidget *parent): QComboBox(parent)
{
    foreach(RamState *state, Ramses::instance()->states()) newState(state);

    connect(Ramses::instance(), &Ramses::newState, this, &StateBox::newState);
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
    return Ramses::instance()->state( this->currentData().toString() );
}

void StateBox::newState(RamState *state)
{
    this->addItem(state->shortName(), state->uuid());

    connect(state, &RamObject::removed, this, &StateBox::stateRemoved);
    connect(state, &RamState::changed, this, &StateBox::stateChanged);
}


void StateBox::stateRemoved(RamObject *o)
{
    for (int i = this->count() -1; i >=0; i--)
    {
        if (this->itemData(i).toString() == o->uuid()) this->removeItem(i);
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
    RamState *state = Ramses::instance()->state( this->itemData(i).toString() );
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
