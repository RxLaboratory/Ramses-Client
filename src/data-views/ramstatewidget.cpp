#include "ramstatewidget.h"

RamStateWidget::RamStateWidget(RamState *state, QWidget *parent):
    RamObjectWidget(state, parent)
{
    _state = state;

    StateEditWidget *sw = new StateEditWidget(state, this);
    setEditWidget(sw);

    setUserEditRole(RamUser::Admin);

    stateChanged();
    connect(state, &RamObject::changed, this, &RamStateWidget::stateChanged);
}

RamState *RamStateWidget::state() const
{
    return _state;
}

void RamStateWidget::stateChanged()
{
    setColor(_state->color());
}
