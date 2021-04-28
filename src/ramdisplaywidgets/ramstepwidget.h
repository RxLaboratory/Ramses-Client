#ifndef RAMSTEPWIDGET_H
#define RAMSTEPWIDGET_H

#include "ramobjectwidget.h"
#include "stepeditwidget.h"

class RamStepWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamStepWidget(RamStep *step, QWidget *parent = nullptr);
    RamStep *step() const;
private slots:
    void stepChanged(RamObject *obj);
private:
    RamStep *m_step;
};

#endif // RAMSTEPWIDGET_H
