#include "ramshotwidget.h"

#include "mainwindow.h"

RamShotWidget::RamShotWidget(RamShot *shot, QWidget *parent) :
    RamObjectWidget(shot, parent)
{
    _shot = shot;
    setUserEditRole(RamUser::Lead);

    ShotEditWidget *sw = new ShotEditWidget(shot, this);
    setEditWidget(sw);
    MainWindow *mw = (MainWindow*)GuiUtils::appMainWindow();
    mw->addShotEditDockWidget(this->dockEditWidget());

}

RamShot *RamShotWidget::shot() const
{
    return _shot;
}
