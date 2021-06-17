#ifndef RAMSHOTWIDGET_H
#define RAMSHOTWIDGET_H

#include <math.h>

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

protected slots:
    void exploreClicked() Q_DECL_OVERRIDE;

private slots:
    void shotChanged();

private:
    RamShot *_shot;

    void completeUi();
    void connectEvents();

    QLabel *m_durationLabel;
    QLabel *m_sequenceLabel;
};

#endif // RAMSHOTWIDGET_H
