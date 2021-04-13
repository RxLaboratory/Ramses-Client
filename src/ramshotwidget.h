#ifndef RAMSHOTWIDGET_H
#define RAMSHOTWIDGET_H

#include "ramobjectwidget.h"
#include "ramses.h"
#include "shoteditwidget.h"

class RamShotWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamShotWidget(RamShot *shot, QWidget *parent = nullptr);
    RamShot *shot() const;

private:
    RamShot *_shot;
};

#endif // RAMSHOTWIDGET_H
