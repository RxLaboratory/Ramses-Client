#ifndef RAMSHOTWIDGET_H
#define RAMSHOTWIDGET_H

#include "ramobjectwidget.h"
#include "ramses.h"
#include "shoteditwidget.h"

/**
 * @brief The RamShotWidget class is a small widget used to show a shot anywhere in the UI
 */
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
