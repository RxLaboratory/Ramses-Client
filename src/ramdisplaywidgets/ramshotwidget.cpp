#include "ramshotwidget.h"

RamShotWidget::RamShotWidget(RamShot *shot, QWidget *parent) :
    RamObjectWidget(shot, parent)
{
    _shot = shot;
    setUserEditRole(RamUser::Lead);

    ShotEditWidget *sw = new ShotEditWidget(shot, this);
    setEditWidget(sw);
    this->dockEditWidget()->setIcon(":/icons/shot");

    setIcon(":/icons/shot");
}

RamShot *RamShotWidget::shot() const
{
    return _shot;
}
