#include "statebox.h"

StateBox::StateBox(QWidget *parent): RamObjectListComboBox(parent)
{
    this->setList(Ramses::instance()->states());
    connect(this, SIGNAL(currentObjectChanged(RamObject*)), this, SLOT(currentStateChanged(RamObject*)));
}

void StateBox::currentStateChanged(RamObject *obj)
{
    if (!obj) return;
    RamState *state = qobject_cast<RamState*>(obj);
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
}
