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

private slots:
    void update() Q_DECL_OVERRIDE;
    void shotChanged(RamObject *o);
    void framesChanged();
    void secondsChanged();

private:
    RamShot *_shot = nullptr;

    bool updating = false;

    void setupUi();
    void connectEvents();

    QDoubleSpinBox *secondsBox;
    QSpinBox *framesBox;
    QComboBox *sequencesBox;

    RamSequence *sequence();
    RamProject *project();
};

#endif // SHOTEDITWIDGET_H
