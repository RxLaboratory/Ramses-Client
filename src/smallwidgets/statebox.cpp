#include "statebox.h"

#include "ramses.h"

StateBox::StateBox(QWidget *parent): RamObjectListComboBox(parent)
{
    this->setList(Ramses::instance()->states());
    connect(this, &RamObjectListComboBox::currentObjectChanged, this, &StateBox::changeCurrentState);
}

void StateBox::changeCurrentState(RamObject *state)
{
    if (!state) return;

    this->setItemData(this->currentIndex(), state->shortName(), Qt::DisplayRole);
    this->repaint();

    QString colorStyle = "background-color: " + state->color().name() + "; ";
    if (state->color().lightness() > 80) colorStyle += "color: #232323;  ";

    QString style = "QComboBox { ";
    style += colorStyle;
    style += "}";


    this->setStyleSheet(style);

    this->setToolTip(state->name());

    emit currentStateChanged(RamState::c( state ));
}
