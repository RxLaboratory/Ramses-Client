#include "ramstatebox.h"

#include "ramses.h"

RamStateBox::RamStateBox(QWidget *parent):
    RamObjectComboBox(parent)
{
    this->setModel(Ramses::instance()->states());
    connect(this, &RamObjectComboBox::currentObjectChanged, this, &RamStateBox::changeCurrentState);
}

void RamStateBox::changeCurrentState(RamObject *state)
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
