#ifndef SHOTEDITWIDGET_H
#define SHOTEDITWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QComboBox>

#include "objecteditwidget.h"
#include "ramses.h"

class ShotEditWidget : public ObjectEditWidget
{
    Q_OBJECT
public:
    ShotEditWidget(RamShot *shot = nullptr, QWidget *parent = nullptr);
    void setShot(RamShot *shot);

protected slots:
    void update() Q_DECL_OVERRIDE;

private slots:
    void shotChanged(RamObject *o);
    void framesChanged();
    void secondsChanged();
    void newSequence(RamSequence *seq);
    void sequenceChanged(RamObject *o);
    void sequenceRemoved(RamSequence *seq);

private:
    RamShot *_shot = nullptr;

    void setupUi();
    void connectEvents();

    QDoubleSpinBox *secondsBox;
    QSpinBox *framesBox;
    QComboBox *sequencesBox;

    RamSequence *sequence();
    RamProject *project();
};

#endif // SHOTEDITWIDGET_H
