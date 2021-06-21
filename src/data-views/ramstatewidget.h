#ifndef RAMSTATEWIDGET_H
#define RAMSTATEWIDGET_H

#include "ramobjectwidget.h"
#include "stateeditwidget.h"

class RamStateWidget : public RamObjectWidget
{
    Q_OBJECT
public:
    RamStateWidget(RamState *state, QWidget *parent = nullptr);
    RamState *state() const;

private slots:
    void stateChanged();

private:
    RamState *_state;
};

#endif // RAMSTATEWIDGET_H
