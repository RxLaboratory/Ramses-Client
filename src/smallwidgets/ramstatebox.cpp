#include "ramstatebox.h"

#include "ramses.h"
#include "duapp/duui.h"

RamStateBox::RamStateBox(QWidget *parent):
    RamObjectComboBox(parent)
{
    this->setMinimumWidth(150);
    this->setModel(Ramses::i()->states());
    connect(this, &RamObjectComboBox::currentObjectChanged, this, &RamStateBox::changeCurrentState);
}

void RamStateBox::update()
{
    changeCurrentState(this->currentObject());
}

void RamStateBox::changeCurrentState(RamObject *state)
{
    if (!state) return;

    this->setItemData(this->currentIndex(), state->shortName(), Qt::DisplayRole);

    QString colorStyle = "background-color: " + state->color().name() + "; ";
    if (state->color().lightness() > 80) colorStyle += "color: #232323;  ";
    colorStyle = "QComboBox { " + colorStyle + "}";

    DuUI::setCSS(this, colorStyle, true);

    this->setToolTip(state->name());
    this->repaint();
    emit currentStateChanged(RamState::c( state ));
}
